/*******************************************************************************
*
*    File name: LCDriverMethods.h
*      Project: LCDriver
*     Language: Visual C++
*  Description: Implementation of all exported methods.
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

#ifndef _LCDRIVERMETHODS_H_
#define _LCDRIVERMETHODS_H_

#include "Buffers.h"
#include "Queue.h"
#include "Timer.h"
#include "Hash.h"
#include "Serialization.h"
#include "Logger.h"
#include "CmdResult.h"
#include "LCDriverThread.h"
#include "BulkHandler.h"
#include "Event.h"

class LcmInterface;
class ZRpcInterface;
class ProtromRpcInterface;
class LoaderRpcInterfaceImpl;
class A2LoaderRpcInterfaceImpl;

typedef void (*ProgressBarCallback_t)(void *Communication_p, uint64 TotalLength, uint64 TransferredLength);

/// <summary>
/// Implementation class.
/// </summary>
class CLCDriverMethods
{
    friend class CLCDriverThread;
public:
    CLCDriverMethods(const char *pchInterfaceId);
    virtual ~CLCDriverMethods();

    int Do_Initialize(void **ppInstance);

    int SetInitialProtocolFamily(TFamily family);
    int ConfigureCommunicationDevice(void *Read_fn, void *Write_fn, void *Cancel_fn);
    int SetMessageCallback(void *Callback_fn);
    int SetProgressCallback(void *Callback_fn);

    int SetPcTimeouts(TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs);
    int GetPcTimeouts(TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs);

    void CancelCurrentLoaderCommand();

    // Loader methods
    int Done_System_LoaderStartupStatus(char *pchVersion, int *piVersionSize, char *pchProtocol, int *piProtocolSize);
    int Do_System_ChangeBaudRate(int iBaudRate);
    int Do_System_Reboot(int iMode);
    int Do_System_Shutdown();
    int Do_System_SupportedCommands(TSupportedCmd *pCmdList, int *piCmdListSize);
    int Do_System_CollectData(int iType, int *piSize, char *pData);
    int Do_System_ExecuteSoftware(const uint32 ExecuteMode, const char *pchDevicePath, int iUseBulk);
    int Do_System_Authenticate(int iType, int *piSize, unsigned char *puchdata);
    int Done_System_GetControlKeys(TSIMLockKeys *pSIMLockKeys);
    int Done_System_GetControlKeysData(int iDataSize, unsigned char *pSIMLockKeysData);
    int Done_System_AuthenticationChallenge(int iDataSize, unsigned char *puchChallengeData);
    int Do_System_SetSystemTime(uint32 EpochTime);
    int Do_System_SwitchCommunicationDevice(uint32 Device, uint32 DeviceParam);
    int Do_System_StartCommRelay(uint32 HostDeviceId, uint32 TargetDeviceId, uint32 ControlDeviceId);
    int Do_System_Deauthenticate(int iType);

    int Do_Flash_ProcessFile(const char *pchPath, const char *pchType, int iUseBulk, int iDeleteBuffers);
    int Do_Flash_ListDevices(TDevices *pDevices, int *piDeviceSize);
    int Do_Flash_DumpArea(const char *pchPathToDump, uint64 uiStart, uint64 uiLength, const char *pchFilePath, uint32 RedundantArea, int iUseBulk);
    int Do_Flash_EraseArea(const char *pchPath, uint64 uiStart, uint64 uiLength);
    int Do_Flash_FlashRaw(const char *pchPath, uint64 uiStart, uint64 uiLength, uint32 uiDevice, int iUseBulk, int iDeleteBuffers);
    int Do_Flash_SetEnhancedArea(const char *pchPathToDump, uint64 uiStart, uint64 uiLength);
    int Do_Flash_SelectLoaderOptions(uint32 uiProperty, uint32 uiValue);

    int Do_FileSystem_VolumeProperties(const char *pchDevicePath, char *pchFSType, int *piFSTypeSize, uint64 *puiSize, uint64 *puiFree);
    int Do_FileSystem_FormatVolume(const char *pchDevicePath);
    int Do_FileSystem_ListDirectory(const char *pchPath, TEntries *pEntries, int *DeviceSize);
    int Do_FileSystem_MoveFile(const char *pchSourcePath, const char *pchDestinationPath);
    int Do_FileSystem_DeleteFile(const char *pchTargetPath);
    int Do_FileSystem_CopyFile(const char *pchSourcePath, int iSourceUseBulk, const char *pchDestinationPath, int iDestinationUseBulk);
    int Do_FileSystem_CreateDirectory(const char *pchTargetPath);
    int Do_FileSystem_Properties(const char *pchTargetPath, uint32 *puiMode, uint64 *puiSize, int *piMTime, int *piATime, int *piCTime);
    int Do_FileSystem_ChangeAccess(const char *pchTargetPath, int iAccess);
    int Do_FileSystem_ReadLoadModuleManifests(const char *pchTargetPath, const char *pchSourcePath);

    int Do_OTP_ReadBits(int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer, int *piDataBufferSize , unsigned char *puchStatusBuffer, int *piStatusBufferSize);
    int Do_OTP_SetBits(int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer);
    int Do_OTP_WriteAndLock(int iOtpId, int iForceWrite);
    int Do_OTP_StoreSecureObject(const char *pchSourcePath, int iDestination, int iUseBulk);

    int Do_ParameterStorage_ReadGlobalDataUnit(const char *pchGdfsId, int iUnit, unsigned char *puchDataBuffer, int *piSize);
    int Do_ParameterStorage_WriteGlobalDataUnit(const char *pchGdfsId, int iUnit, const unsigned char *puchDataBuffer, int iSize);
    int Do_ParameterStorage_ReadGlobalDataSet(const char *pchGdfsId, const char *pchPath, int iUseBulk);
    int Do_ParameterStorage_WriteGlobalDataSet(const char *pchGdfsId, const char *pchPath, int iUseBulk);
    int Do_ParameterStorage_EraseGlobalDataSet(const char *pchGdfsId);

    int Do_Security_SetDomain(int iDomain);
    int Do_Security_GetDomain(int *piWrittenDomain);
    int Do_Security_GetProperties(int iUnitId, int *piSize, unsigned char *puchDataBuffer);
    int Do_Security_SetProperties(int iUnitId, int iSize, const unsigned char *puchDataBuffer);
    int Do_Security_BindProperties();
    int Do_Security_InitARBTable(int iType, int iLength, const unsigned char *puarbdata);
    int Do_Security_WriteRPMBKey(uint32 uiDev_id, uint32 uiCommercial);

    // A2 Protocol Methods
    int Do_A2_System_Shutdown();
    int Do_A2_System_LoaderVersion(char *pchLoaderVersion, int *piSize, int iTargetCPU);
    int Do_A2_System_LoaderOnLoader(const char *pchPath, int iPLOffset, int iHLOffset, int iTargetCPU);
    int Do_A2_System_Reset(int iTimeout);

    int Do_A2_Flash_ProgramFlash(const char *pchPath, int iUseSpeedFlash);
    int Do_A2_Flash_EraseFlash();

    int Done_A2_Control_LoaderStarted();

    // Z Protocol Methods
    int Do_Z_SetInServiceMode(unsigned int *puiChipId);
    int Do_Z_SetBaudrate(int iBaudrate);
    int Do_Z_Exit_Z_Protocol();

    // PROTROM Protocol Methods
    int Do_PROTROM_DownloadLoader(const char *pchPath, int iPLOffset, int iHLOffset, int iContinueProtRom);
    int Do_PROTROM_ReadSecurityData(uint8 uiSecDataId, unsigned char *puchDataBuffer, int *piDataLength);

    int Do_SwitchProtocolFamily(TFamily family);

private:
    // Members
    Buffers        *m_pBuffers;
    Queue          *m_pQueue;
    Timer          *m_pTimer;
    Hash           *m_pHash;
    Serialization  *m_pSerialization;
    Logger         *m_pLogger;
    BulkHandler    *m_pBulkHandler;


    HashDevice_t           *m_pHashDevice;
    CommunicationDevice_t  *m_pCommunicationDevice;

    // Utility setup methods
    ErrorCode_e SetupTimers(TimersInterface_t *pTimerFunctions);
    ErrorCode_e SetupBuffers(BuffersInterface_t *pBufferFunctions);
    ErrorCode_e SetupQueues(QueueInterface_t *pQueueFunctions);
    ErrorCode_e SetupHash();

    CmdResult                *m_pCmdResult;
    LcmInterface             *m_pLcmInterface;
    ZRpcInterface            *m_pZRpcFunctions;
    ProtromRpcInterface      *m_pProtromRpcFunctions;
    LoaderRpcInterfaceImpl   *m_pLoaderRpcFunctions;
    A2LoaderRpcInterfaceImpl *m_pA2LoaderRpcFunctions;
    CLCDriverThread          *m_pMainThread;
    CCriticalSectionObject    LCDMethodsCS;

    //------------------------------------------
    // Static methods for support modules: timers, hash, buffers and queue
    //------------------------------------------
public:
    static ErrorCode_e TimerInit(void *pObject, uint32 uiTimers);
    static uint32 TimerGet(void *pObject, Timer_t *pTimer);
    static ErrorCode_e TimerRelease(void *pObject, uint32 uiTimerKey);
    static uint32 TimerReadTime(void *pObject, uint32 uiTimerKey);
    static uint32 TimerGetSystemTime(void *pObject);

    static void HashCancel(void *pObject, void **ppHashDevice);
    static void HashCalculate(void *pObject, HashType_e Type, void *pData, const uint32 uiLength, uint8 *pHash, HashCallback_fn fnCallback, void *pParam);
    static uint32 HashGetLength(void *pObject, HashType_e Type);

    static ErrorCode_e BuffersInit(void *pObject);
    static void *BufferAllocate(void *pObject, int iBufferSize);
    static ErrorCode_e BufferRelease(void *pObject, void *pBuffer, int iBufferSize);
    static uint32 BuffersAvailable(void *pObject, int iBufferSize);
    static void BuffersDeinit(void *pObject);

    static void QueueCreate(void *pObject, void **const ppQueue, const uint32 uiMaxLength, void (*pDestroyElement)(void *pElement));
    static void QueueDestroy(void *pObject, void **const ppQueue);
    static ErrorCode_e QueueEnqueue(void *pObject, void *const ppQueue, void *const pValue);
    static void *QueueDequeue(void *pObject, void *const ppQueue);
    static QueueCallback_fn QueueSetCallback(void *pObject, void *const pQueue, const QueueCallbackType_e Type, const QueueCallback_fn fnCallback, void *const pParam);
    static boolean QueueIsEmpty(void *pObject, const void *const pQueue);
    static boolean QueueIsMember(void *pObject, const void *const pQueue, void *pValue, boolean(*Match)(void *pValue1, void *pValue2));
    static int QueueGetNrOfElements(void *pObject, const void *const pQueue);
    static void RQueueCreate(void *pObject, void **const ppQueue, const uint32 uiMaxLength, void (*pDestroyElement)(void *pElement));
    static void RQueueDestroy(void *pObject, void **const ppQueue);
    static ErrorCode_e RQueueEnqueue(void *pObject, void *const pQueue, void *const pValue);
    static void *RQueueDequeue(void *pObject, void *const pQueue);
    static QueueCallback_fn RQueueSetCallback(void *pObject, void *const pQueue, const QueueCallbackType_e Type, const QueueCallback_fn fnCallback, void *const pParam);
    static boolean RQueueIsEmpty(void *pObject, const void *const pQueue);
    static boolean RQueueIsMember(void *pObject, const void *const pQueue, void *pValue, boolean(*Match)(void *pValue1, void *pValue2));
    static int RQueueGetNrOfElements(void *pObject, const void *const pQueue);

    //-----------------------------------------
    //  Command Execution Handler callbacks
    //-----------------------------------------
    static ErrorCode_e  CEHCallbackFunction(void *pObject, CommandData_t *pCmdData);
    static ErrorCode_e  CEH_PROTROM_CallbackFunction(void *pObject, CommandData_t *pCmdData);
    static ErrorCode_e  CEH_Z_CallbackFunction(void *pObject, CommandData_t *pCmdData);
    static ErrorCode_e  CEH_A2_CallbackFunction(void *pObject, CommandData_t *pCmdData);

    //-----------------------------------------
    //  Bulk Transfer Protocol callbacks
    //-----------------------------------------
    static void   BulkCommandReqCallback(void *pObject, uint16 uiSession, uint32 uiChunkSize, uint64 uiOffset, uint32 uiLength, boolean bAckRead);
    void          Do_BulkCommandReqCallback(uint16 uiSession, uint32 uiChunkSize, uint64 uiOffset, uint32 uiLength);

    static void   BulkDataReqCallback(void *pObject, uint16 uiSession, uint32 uiChunkSize, uint64 uiOffset, uint32 uiLength, uint64 uiTotalLength, uint32 uiTransferedLength);
    void          Do_BulkDataReqCallback(uint16 uiSession, uint32 uiChunkSize, uint64 uiOffset, uint32 uiLength, uint64 uiTotalLength, uint32 uiTransferedLength);

    static void   BulkDataEndOfDumpCallback(void *pObject);
    void          Do_BulkDataEndOfDumpCallback();

    static void   BulkReleaseBuffers(void *pObject, TL_BulkVectorList_t *BulkVector_p);


    int           MapLcmError(int error);

    void          SignalError(int error);
    void          AddEvent(Event *event);

    //-----------------------------------------
    //  Other callbacks
    //-----------------------------------------
    ProgressBarCallback_t m_ProgressBarUpdate;
    void UpdateBulkProgress();
    //-----------------------------------------

private:
    char *m_pchId;

    static const char *const BULK_PATH;
    uint64 m_uiBulkLength;
    uint64 m_uiBulkTransferred;

    CSemaphoreQueue         m_EventQueue;
    CCriticalSectionObject  m_GetNextCodeCS;

    CCriticalSectionObject  m_SetTimeoutCS;
    TLCDriverTimeouts       m_Timeouts;

    //-----------------------------------------
    //  String and vector copy functions.
    //-----------------------------------------
    template<class T, class U>
    void CopyVectorToArray(const std::vector<T> &Source, U *pDestination, int *piSize);

    void CopyStringToArray(std::string &Source, char *pDestination, int *piSize);

    //-----------------------------------------
    //  Wait functions
    //-----------------------------------------
    int WaitForEvent(uint32 event, int Group = 0, int Command = 0);
    int WaitForPROTROMResponseOrCancelOrTimeout(int iReceivePdu);

    //-----------------------------------------
    //  State variables
    //-----------------------------------------
    Family_t      m_CurrentProtocolFamily;
    Do_CEH_Call_t m_CurrentCEHCallback;
    int           m_iBulkProtocolMode;

    //-----------------------------------------
    //  General functions
    //-----------------------------------------
    int  IsMainThreadAlive();
};

#endif // _LCDRIVERMETHODS_H_
