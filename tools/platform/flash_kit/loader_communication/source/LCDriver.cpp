/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include "LCDriver.h"
#include "lcdriver_error_codes.h"
#include "LCDriverInterface.h"
#include "Error.h"
#include "error_codes_desc.h"

#define VERIFY_CONTEXT(c) \
    do { \
        if ((c) == NULL) \
            return INTERFACE_OBJECT_POINTER_NULL; \
        if ((c)->m_pObject == NULL) \
            return INTERFACE_LC_METHODS_OBJECT_NULL; \
    } while (0)

#define VERIFY_CONTEXT_STARTED(c) \
    do { \
        VERIFY_CONTEXT(c); \
        if (!(c)->isStarted) \
            return INTERFACE_CONTEXT_NOT_STARTED; \
    } while (0)

/************************************************************************************************
* API functions for LCDriver functionalities
************************************************************************************************/
LCDRIVER_API int __cdecl Initialize(LCDContext *pContext, const char *InterfaceId, void *Read_fn, void *Write_fn, void *Cancel_fn, void **Instance, void *Message_fn, const char *LCMLibPath, void *ProgressBar_fn)
{
    SetLCMLibPath(LCMLibPath);

    int Result = CreateContext(pContext, InterfaceId);

    if (0 != Result) {
        return Result;
    }

    // check if we have context that was previously started
    if (!(*pContext)->isStarted) {
        ConfigureCommunicationDevice(*pContext, Read_fn, Write_fn, Cancel_fn);
        SwitchProtocolFamily(*pContext, R15_PROTOCOL_FAMILY);
        SetMessageCallback(*pContext, Message_fn);
        SetProgressCallback(*pContext, ProgressBar_fn);
        Result = StartContext(*pContext, Instance);
        return Result;
    } else {
        return 0;
    }
}

LCDRIVER_API int __cdecl SetLCMLibPath(const char *LCMLibPath)
{
    return (LcmInterface::SetLCMLibPath(LCMLibPath));
}

LCDRIVER_API int __cdecl CreateContext(LCDContext *pContext, const char *InterfaceId)
{
    CLockCS CsLock(CLCDriverInterface::InitializationCS);

    CLCDriverInterface *Context = NULL;
    int ReturnValue = E_SUCCESS;

    // Check pointer to pointer
    VERIFY(NULL != pContext, INTERFACE_OBJECT_POINTER_TO_POINTER_NULL);

    // Check if CLCDriverInterface already exists
    VERIFY(NULL == *pContext, INTERFACE_OBJECT_ALREADY_EXISTS);

    // Check context id
    VERIFY(NULL != InterfaceId, INTERFACE_INTERFACEID_POINTER_NULL);

    // Create new CLCDriverInterface
    Context = new CLCDriverInterface();
    VERIFY(NULL != Context, INTERFACE_COULD_NOT_CREATE_IFC_OBJECT);

    strcpy_s(Context->m_szObjectId, InterfaceId);

    // Check in the object list if this particular object has been created earlier.
    Context->m_pObject = CLCDriverInterface::FindObject(Context->m_szObjectId);

    // An object was found in the list
    if (Context->m_pObject == NULL) {
        // Create new CLCDriverMethods
        Context->m_pObject = new CLCDriverMethods(InterfaceId);
        VERIFY(NULL != Context->m_pObject, INTERFACE_COULD_NOT_CREATE_OBJECT);
    }

    // Add the object to the list to increase the reference counter
    ReturnValue = CLCDriverInterface::AddObject(Context->m_pObject, Context->m_szObjectId);
    VERIFY(0 == ReturnValue, INTERFACE_COULD_NOT_ADD_OBJECT_TO_LIST);

    *pContext = Context;

    return E_SUCCESS;
ErrorExit:

    // Delete the CLCDriverInterface
    if (Context != NULL) {
        if (Context->m_pObject != NULL) {
            delete Context->m_pObject;
            Context->m_pObject = NULL;
        }

        delete Context;
    }

    return ReturnValue;
}

LCDRIVER_API int __cdecl SwitchProtocolFamily(LCDContext Context, TFamily Family)
{
    VERIFY_CONTEXT(Context);

    if (Context->isStarted) {
        return Context->m_pObject->Do_SwitchProtocolFamily(Family);
    } else {
        return Context->m_pObject->SetInitialProtocolFamily(Family);
    }
}

LCDRIVER_API int __cdecl ConfigureCommunicationDevice(LCDContext Context, void *Read_fn, void *Write_fn, void *Cancel_fn)
{
    VERIFY_CONTEXT(Context);

    return(Context->m_pObject->ConfigureCommunicationDevice(Read_fn, Write_fn, Cancel_fn));
}

LCDRIVER_API int __cdecl SetMessageCallback(LCDContext Context, void *Callback_fn)
{
    VERIFY_CONTEXT(Context);

    return(Context->m_pObject->SetMessageCallback(Callback_fn));
}

LCDRIVER_API int __cdecl SetProgressCallback(LCDContext Context, void *Callback_fn)
{
    VERIFY_CONTEXT(Context);

    Context->m_pObject->SetProgressCallback(Callback_fn);
    return 0;
}

LCDRIVER_API int __cdecl StartContext(LCDContext Context, void **Instance)
{
    VERIFY_CONTEXT(Context);

    int Result = Context->m_pObject->Do_Initialize(Instance);

    if (0 == Result) {
        Context->isStarted = true;
    }

    return Result;
}

LCDRIVER_API int __cdecl DestroyContext(LCDContext *pContext)
{
    LCDContext Context = *pContext;
    VERIFY_CONTEXT(Context);

    delete Context;
    *pContext = NULL;

    return 0;
}

LCDRIVER_API int __cdecl Close(LCDContext Context)
{
    return DestroyContext(&Context);
}

LCDRIVER_API int __cdecl SetTimeouts(LCDContext Context, TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs)
{
    VERIFY_CONTEXT(Context);

    return Context->m_pObject->SetPcTimeouts(R15_TOs, LCD_TOs);
}

LCDRIVER_API int __cdecl GetTimeouts(LCDContext Context, TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs)
{
    VERIFY_CONTEXT(Context);

    return Context->m_pObject->GetPcTimeouts(R15_TOs, LCD_TOs);
}

LCDRIVER_API int __cdecl CancelCurrentLoaderCommand(LCDContext Context)
{
    VERIFY_CONTEXT(Context);

    Context->m_pObject->CancelCurrentLoaderCommand();

    return 0;
}

LCDRIVER_API void __cdecl GetLoaderErrorDescription(uint32 ErrorNr, uint8 *ShortDescription, uint8 *LongDescription, uint32 ShorDescrBufLen, uint32 LongDescrBufLen)
{
    GetLoaderErrorDescription_Call(ErrorNr, ShortDescription, LongDescription, ShorDescrBufLen, LongDescrBufLen);
}

/************************************************************************************************
* API functions for loader commands
************************************************************************************************/
LCDRIVER_API int __cdecl System_LoaderStartupStatus(LCDContext Context, char *pchVersion, int *piVersionSize, char *pchProtocol, int *piProtocolSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return(Context->m_pObject->Done_System_LoaderStartupStatus(pchVersion, piVersionSize, pchProtocol, piProtocolSize));
}

LCDRIVER_API int __cdecl System_ChangeBaudrate(LCDContext Context, int iBaudRate)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_ChangeBaudRate(iBaudRate);
}

LCDRIVER_API int __cdecl System_Reboot(LCDContext Context, int iMode)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_Reboot(iMode);
}

LCDRIVER_API int __cdecl System_Shutdown(LCDContext Context)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_Shutdown();
}

LCDRIVER_API int __cdecl System_SupportedCommands(LCDContext Context, TSupportedCmd *pCmdList, int *piCmdListSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_SupportedCommands(pCmdList, piCmdListSize);
}

LCDRIVER_API int __cdecl System_CollectData(LCDContext Context, int iType, int *piSize, char *pData)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_CollectData(iType,  piSize, pData);
}

LCDRIVER_API int __cdecl System_ExecuteSoftware(LCDContext Context, const uint32 ExecuteMode, const char *pchDevicePath, int iUseBulk)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_ExecuteSoftware(ExecuteMode, pchDevicePath, iUseBulk);
}

LCDRIVER_API int __cdecl System_Authenticate(LCDContext Context, int iType, int *piSize, unsigned char *puchdata)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_Authenticate(iType, piSize, puchdata);
}

LCDRIVER_API int __cdecl System_Deauthenticate(LCDContext Context, int iType)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_Deauthenticate(iType);
}

LCDRIVER_API int __cdecl System_GetControlKeys(LCDContext Context, TSIMLockKeys *pSIMLockKeys)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Done_System_GetControlKeys(pSIMLockKeys);
}

LCDRIVER_API int __cdecl System_GetControlKeysData(LCDContext Context, int iDataSize, unsigned char *pSIMLockKeysData)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Done_System_GetControlKeysData(iDataSize, pSIMLockKeysData);
}

LCDRIVER_API int __cdecl System_AuthenticationChallenge(LCDContext Context, unsigned char *puchChallengeData, int iDataSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Done_System_AuthenticationChallenge(iDataSize, puchChallengeData);
}

LCDRIVER_API int __cdecl System_SetSystemTime(LCDContext Context, uint32 uiEpochTime)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_SetSystemTime(uiEpochTime);
}

LCDRIVER_API int __cdecl System_SwitchCommunicationDevice(LCDContext Context, uint32 uiDevice, uint32 uiDeviceParam)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_SwitchCommunicationDevice(uiDevice, uiDevice);
}

LCDRIVER_API int __cdecl System_StartCommRelay(LCDContext Context, uint32 HostDeviceId, uint32 TargetDeviceId, uint32 ControlDeviceId)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_System_StartCommRelay(HostDeviceId, TargetDeviceId, ControlDeviceId);
}

LCDRIVER_API int __cdecl Flash_ProcessFile(LCDContext Context, const char *pchPath, const char *pchType, int iUseBulk, int iDeleteBuffers)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Flash_ProcessFile(pchPath, pchType, iUseBulk, iDeleteBuffers);
}

LCDRIVER_API int __cdecl Flash_ListDevices(LCDContext Context, TDevices *pDevices, int *piDeviceSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Flash_ListDevices(pDevices, piDeviceSize);
}

LCDRIVER_API int __cdecl Flash_DumpArea(LCDContext Context, const char *pchPathToDump, uint64 uiStart, uint64 uiLength, const char *pchFilePath, uint32 uiRedundantArea, int iUseBulk)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Flash_DumpArea(pchPathToDump, uiStart, uiLength, pchFilePath, uiRedundantArea, iUseBulk);
}

LCDRIVER_API int __cdecl Flash_EraseArea(LCDContext Context, const char *pchPath, uint64 uiStart, uint64 uiLength)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Flash_EraseArea(pchPath, uiStart, uiLength);
}

LCDRIVER_API int __cdecl Flash_FlashRaw(LCDContext Context, const char *pchPath, uint64 uiStart, uint64 uiLength, uint32 uiDevice, int iUseBulk, int iDeleteBuffers)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Flash_FlashRaw(pchPath, uiStart, uiLength, uiDevice, iUseBulk, iDeleteBuffers);
}

LCDRIVER_API int __cdecl Flash_SetEnhancedArea(LCDContext Context, const char *pchPathToDump, uint64 uiStart, uint64 uiLength)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Flash_SetEnhancedArea(pchPathToDump, uiStart, uiLength);
}

LCDRIVER_API int __cdecl Flash_SelectLoaderOptions(LCDContext Context, uint32 uiProperty, uint32 uiValue)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Flash_SelectLoaderOptions(uiProperty, uiValue);
}

LCDRIVER_API int __cdecl FileSystem_VolumeProperties(LCDContext Context, const char *pchDevicePath, char *pchFSType, int *piFSTypeSize, uint64 *puiSize, uint64 *puiFree)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_VolumeProperties(pchDevicePath, pchFSType, piFSTypeSize, puiSize, puiFree);
}

LCDRIVER_API int __cdecl FileSystem_FormatVolume(LCDContext Context, const char *pchDevicePath)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_FormatVolume(pchDevicePath);
}

LCDRIVER_API int __cdecl FileSystem_ListDirectory(LCDContext Context, const char *pchPath, TEntries *pEntries, int *piDeviceSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_ListDirectory(pchPath, pEntries, piDeviceSize);
}

LCDRIVER_API int __cdecl FileSystem_MoveFile(LCDContext Context, const char *pchSourcePath,  const char *pchDestinationPath)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_MoveFile(pchSourcePath, pchDestinationPath);
}

LCDRIVER_API int __cdecl FileSystem_DeleteFile(LCDContext Context, const char *pchTargetPath)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_DeleteFile(pchTargetPath);
}

LCDRIVER_API int __cdecl FileSystem_CopyFile(LCDContext Context, const char *pchSourcePath, int iSourceUseBulk, const char *pchDestinationPath, int iDestinationUseBulk)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_CopyFile(pchSourcePath, iSourceUseBulk, pchDestinationPath, iDestinationUseBulk);
}

LCDRIVER_API int __cdecl FileSystem_CreateDirectory(LCDContext Context, const char *pchTargetPath)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_CreateDirectory(pchTargetPath);
}

LCDRIVER_API int __cdecl FileSystem_Properties(LCDContext Context, const char *pchTargetPath, uint32 *puiMode, uint64 *puiSize, int *piMTime, int *piATime, int *piCTime)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_Properties(pchTargetPath, puiMode, puiSize, piMTime, piATime, piCTime);
}

LCDRIVER_API int __cdecl FileSystem_ChangeAccess(LCDContext Context, const char *pchTargetPath, int iAccess)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_ChangeAccess(pchTargetPath, iAccess);
}

LCDRIVER_API int __cdecl FileSystem_ReadLoadModuleManifests(LCDContext Context, const char *pchTargetPath, const char *pchSourcePath)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_FileSystem_ReadLoadModuleManifests(pchTargetPath, pchSourcePath);
}

LCDRIVER_API int __cdecl OTP_ReadBits(LCDContext Context, int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer, int *piDataBufferSize, unsigned char *puchStatusBuffer, int *piStatusBufferSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_OTP_ReadBits(iOtpId, iBitStart, iBitLength, puchDataBuffer, piDataBufferSize, puchStatusBuffer, piStatusBufferSize);
}

LCDRIVER_API int __cdecl OTP_SetBits(LCDContext Context, int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_OTP_SetBits(iOtpId, iBitStart, iBitLength, puchDataBuffer);
}

LCDRIVER_API int __cdecl OTP_WriteAndLock(LCDContext Context, int iOtpId, int iForceWrite)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_OTP_WriteAndLock(iOtpId, iForceWrite);
}

LCDRIVER_API int __cdecl ParameterStorage_ReadGlobalDataUnit(LCDContext Context, const char *pchGdfsId, int iUnit, unsigned char *puchDataBuffer, int *piSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_ParameterStorage_ReadGlobalDataUnit(pchGdfsId, iUnit, puchDataBuffer, piSize);
}

LCDRIVER_API int __cdecl ParameterStorage_WriteGlobalDataUnit(LCDContext Context, const char *pchGdfsId, int iUnit, unsigned char *puchDataBuffer, int iSize)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_ParameterStorage_WriteGlobalDataUnit(pchGdfsId, iUnit, puchDataBuffer, iSize);
}

LCDRIVER_API int __cdecl ParameterStorage_ReadGlobalDataSet(LCDContext Context, const char *pchGdfsId, const char *pchPath, int iUseBulk)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_ParameterStorage_ReadGlobalDataSet(pchGdfsId, pchPath, iUseBulk);
}

LCDRIVER_API int __cdecl ParameterStorage_WriteGlobalDataSet(LCDContext Context, const char *pchGdfsId, const char *pchPath, int iUseBulk)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_ParameterStorage_WriteGlobalDataSet(pchGdfsId, pchPath, iUseBulk);
}

LCDRIVER_API int __cdecl ParameterStorage_EraseGlobalDataSet(LCDContext Context, const char *pchGdfsId)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_ParameterStorage_EraseGlobalDataSet(pchGdfsId);
}

LCDRIVER_API int __cdecl Security_SetDomain(LCDContext Context, int iDomain)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Security_SetDomain(iDomain);
}

LCDRIVER_API int __cdecl Security_GetDomain(LCDContext Context, int *piWrittenDomain)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Security_GetDomain(piWrittenDomain);
}

LCDRIVER_API int __cdecl Security_GetProperties(LCDContext Context, int iUnitId, int *piSize, unsigned char *puchDataBuffer)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Security_GetProperties(iUnitId, piSize, puchDataBuffer);
}

LCDRIVER_API int __cdecl Security_SetProperties(LCDContext Context, int iUnitId, int iSize, unsigned char *puchDataBuffer)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Security_SetProperties(iUnitId, iSize, puchDataBuffer);
}

LCDRIVER_API int __cdecl Security_BindProperties(LCDContext Context)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Security_BindProperties();
}

LCDRIVER_API int __cdecl Security_StoreSecureObject(LCDContext Context, const char *pchSourcePath, int iDestination, int iUseBulk)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_OTP_StoreSecureObject(pchSourcePath, iDestination, iUseBulk);
}

LCDRIVER_API int __cdecl Security_InitARBTable(LCDContext Context, int iType, int iLength, unsigned char *puarbdata)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Security_InitARBTable(iType, iLength, puarbdata);
}

LCDRIVER_API int __cdecl Security_WriteRPMBKey(LCDContext Context, uint32 uiDev_id, uint32 uiCommercial)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Security_WriteRPMBKey(uiDev_id, uiCommercial);
}

LCDRIVER_API int __cdecl A2_System_Shutdown(LCDContext Context)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_A2_System_Shutdown();
}

LCDRIVER_API int __cdecl A2_System_LoaderVersion(LCDContext Context, char *pchLoaderVersion, int *piSize, int iTargetCPU)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_A2_System_LoaderVersion(pchLoaderVersion, piSize, iTargetCPU);
}

LCDRIVER_API int __cdecl A2_System_LoaderOnLoader(LCDContext Context, const char *pchPath, int iPLOffset, int iHLOffset, int iTargetCPU)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_A2_System_LoaderOnLoader(pchPath, iPLOffset, iHLOffset, iTargetCPU);
}

LCDRIVER_API int __cdecl A2_System_Reset(LCDContext Context, int iTimeout)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_A2_System_Reset(iTimeout);
}

LCDRIVER_API int __cdecl A2_Flash_ProgramFlash(LCDContext Context, const char *pchPath, int iUseSpeedFlash)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_A2_Flash_ProgramFlash(pchPath, iUseSpeedFlash);
}

LCDRIVER_API int __cdecl A2_Flash_EraseFlash(LCDContext Context)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_A2_Flash_EraseFlash();
}

LCDRIVER_API int __cdecl A2_Control_LoaderStarted(LCDContext Context)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Done_A2_Control_LoaderStarted();
}

LCDRIVER_API int __cdecl SetProtocolFamily(LCDContext Context, const char *pchFamily)
{
    VERIFY_CONTEXT_STARTED(Context);

    TFamily newFamily;

    if (0 == strcmp(pchFamily, "R15_FAMILY")) {
        newFamily = R15_PROTOCOL_FAMILY;
    } else if (0 == strcmp(pchFamily, "PROTROM_FAMILY")) {
        newFamily = PROTROM_PROTOCOL_FAMILY;
    } else if (0 == strcmp(pchFamily, "NOPROT")) {
        newFamily = Z_PROTOCOL_FAMILY;
    } else if (0 == strcmp(pchFamily, "A2_FAMILY")) {
        newFamily = A2_PROTOCOL_FAMILY;
    } else {
        return INVALID_INPUT_PARAMETERS;
    }

    return Context->m_pObject->Do_SwitchProtocolFamily(newFamily);
}

LCDRIVER_API int __cdecl Z_SetInServiceMode(LCDContext Context, unsigned int *puiChipId)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Z_SetInServiceMode(puiChipId);
}

LCDRIVER_API int __cdecl Z_SetBaudrate(LCDContext Context, int iBaudrate)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Z_SetBaudrate(iBaudrate);
}

LCDRIVER_API int __cdecl Z_Exit_Z_Protocol(LCDContext Context)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_Z_Exit_Z_Protocol();
}

LCDRIVER_API int __cdecl PROTROM_DownloadLoader(LCDContext Context, const char *pchPath, int iPLOffset, int iHLOffset, int iContinueProtRom)
{
    VERIFY_CONTEXT_STARTED(Context);

    return Context->m_pObject->Do_PROTROM_DownloadLoader(pchPath, iPLOffset, iHLOffset, iContinueProtRom);
}

LCDRIVER_API int __cdecl PROTROM_ReadSecurityData(LCDContext Context, unsigned char uiSecDataId, unsigned char *puchDataBuffer, int *piDataLength)
{
    VERIFY_CONTEXT_STARTED(Context);

    if (NULL == puchDataBuffer || 0 == piDataLength) {
        return INVALID_INPUT_PARAMETERS;
    }

    return Context->m_pObject->Do_PROTROM_ReadSecurityData(uiSecDataId, puchDataBuffer, piDataLength);
}
