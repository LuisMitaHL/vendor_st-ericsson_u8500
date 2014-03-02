/*********************************************************************************************
*
*    File name: LCDriverMethods.cpp
*      Project: LoaderCommunicationDriver
*     Language: Visual C++
*  Description: Implementation of all exported methods.
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*********************************************************************************************/

#include "LCDriverMethods.h"
#include "lcdriver_error_codes.h"
#include "LCDriverThread.h"
#include "Error.h"
#include "LcmInterface.h"
#include "ZRpcInterface.h"
#include "ProtromRpcInterface.h"
#include "commands_impl.h"
#include "a2_commands_impl.h"
#include "Event.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#include <cstdlib>
#endif
#include <sys/stat.h>
#include <sys/types.h>

const char *const CLCDriverMethods::BULK_PATH = "/COMM";

// LCM timeouts structure to avoid LCD interface changes after removing bulk session end timeout
struct LcmR15Timeouts {
    uint32 TCACK;
    uint32 TBCR;
    uint32 TBDR;
};

extern ListDevice_t Devices[];
extern uint32 DevicesNumber;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/// <summary>
/// Constructor.
/// </summary>
/// <param name="pchInterfaceId">A zero terminated string containing the InterfaceId.</param>
CLCDriverMethods::CLCDriverMethods(const char *pchInterfaceId):
    m_EventQueue(256)
{
    m_pchId = new char[strlen(pchInterfaceId) + 1];
    strcpy_s(m_pchId, strlen(pchInterfaceId) + 1, pchInterfaceId);

    m_pCommunicationDevice = new CommunicationDevice_t;
    m_pCmdResult           = new CmdResult();
    m_pLcmInterface        = new LcmInterface();
    m_pZRpcFunctions       = new ZRpcInterface(m_pCmdResult, m_pLcmInterface);
    m_pProtromRpcFunctions = new ProtromRpcInterface(m_pCmdResult, m_pLcmInterface);
    m_pLoaderRpcFunctions  = new LoaderRpcInterfaceImpl(this, m_pCmdResult, m_pLcmInterface);
    m_pA2LoaderRpcFunctions = new A2LoaderRpcInterfaceImpl(this, m_pCmdResult, m_pLcmInterface);

    m_pTimer         = new Timer();
    m_pHash          = new Hash();
    m_pQueue         = new Queue();
    m_pBuffers       = new Buffers();
    m_pSerialization = new Serialization();
    m_pLogger        = new Logger(m_pLcmInterface->getLCMContext());
    m_pBulkHandler   = new BulkHandler(this, m_pBuffers, m_pLcmInterface, m_pLogger);

    // default values for timeouts
    m_Timeouts.uiRTO = 0xFFFFFFFF; // Typically 5000
    m_Timeouts.uiSTO = 0xFFFFFFFF; // Typically 30000 i.e. 30 secs

    m_CurrentProtocolFamily = R15_FAMILY;
    m_iBulkProtocolMode     = BULK_INACTIVE;

    m_ProgressBarUpdate  = 0;
    m_uiBulkLength       = 0;
    m_uiBulkTransferred  = 0;
    m_pMainThread        = 0;
    m_pHashDevice        = 0;
    m_CurrentCEHCallback = 0;
}

/// <summary>
/// Destructor.
/// </summary>
CLCDriverMethods::~CLCDriverMethods()
{
    m_EventQueue.SignalEvent();
    CLockCS lock(LCDMethodsCS);
    OS::Sleep(200);

    if (0 != m_pMainThread) {
        m_pMainThread->EndCaptiveThread();
    }

    delete m_pMainThread;

    if (0 != m_pLcmInterface) {
        m_pLcmInterface->CommunicationShutdown();
    }

    delete m_pLcmInterface;
    delete m_pCmdResult;
    delete m_pZRpcFunctions;
    delete m_pProtromRpcFunctions;
    delete m_pLoaderRpcFunctions;
    delete m_pA2LoaderRpcFunctions;

    delete m_pCommunicationDevice;
    delete m_pTimer;
    delete m_pBuffers;
    delete m_pHash;
    delete m_pQueue;
    delete m_pSerialization;

    delete m_pLogger;
    delete m_pBulkHandler;
    delete[] m_pchId;
}
//----------------------------------------
// Static callback functions for intercept only
//----------------------------------------

//----------------------------------------
// Static methods
//----------------------------------------

ErrorCode_e CLCDriverMethods::TimerInit(void *pObject, uint32 uiTimers)
{
    Timer *pTimer = static_cast<Timer *>(pObject);
    return pTimer->Init(uiTimers);
}

uint32 CLCDriverMethods::TimerGet(void *pObject, Timer_t *pTimer)
{
    Timer *pTimerLocal = static_cast<Timer *>(pObject);
    return pTimerLocal->Get(pTimer);
}

ErrorCode_e CLCDriverMethods::TimerRelease(void *pObject, uint32 uiTimerKey)
{
    Timer *pTimer = static_cast<Timer *>(pObject);
    return pTimer->Release(uiTimerKey);
}

uint32 CLCDriverMethods::TimerReadTime(void *pObject, uint32 uiTimerKey)
{
    Timer *pTimer = static_cast<Timer *>(pObject);
    return pTimer->ReadTime(uiTimerKey);
}

uint32 CLCDriverMethods::TimerGetSystemTime(void *pObject)
{
    Timer *pTimer = static_cast<Timer *>(pObject);
    return pTimer->GetSystemTime();
}

//--------------------------------------------
void CLCDriverMethods::HashCancel(void *pObject, void **ppHashDevice)
{
    Hash *pHashObject = static_cast<Hash *>(pObject);
    pHashObject->Cancel((HashDevice_t **)ppHashDevice);
}

void CLCDriverMethods::HashCalculate(void *pObject, HashType_e Type, void *pData, const uint32 uiLength, uint8 *pHash, HashCallback_fn fnCallback, void *pParam)
{
    Hash *pHashObject = static_cast<Hash *>(pObject);
    pHashObject->Calculate(Type, pData, uiLength, pHash, fnCallback, pParam);
}

//----------------------------------------

ErrorCode_e CLCDriverMethods::BuffersInit(void *pObject)
{
    Buffers *pBufferObject = static_cast<Buffers *>(pObject);
    return pBufferObject->Init();
}

void *CLCDriverMethods::BufferAllocate(void *pObject, int iBufferSize)
{
    Buffers *pBufferObject = static_cast<Buffers *>(pObject);
    return pBufferObject->Allocate(iBufferSize);
}

ErrorCode_e CLCDriverMethods::BufferRelease(void *pObject, void *pBuffer, int iBufferSize)
{
    Buffers *pBufferObject = static_cast<Buffers *>(pObject);
    return pBufferObject->Release(pBuffer, iBufferSize);
}

uint32 CLCDriverMethods::BuffersAvailable(void *pObject, int iBufferSize)
{
    Buffers *pBufferObject = static_cast<Buffers *>(pObject);
    return pBufferObject->Available(iBufferSize);
}

void CLCDriverMethods::BuffersDeinit(void *pObject)
{
    Buffers *pBufferObject = static_cast<Buffers *>(pObject);
    pBufferObject->Deinit();
}

//----------------------------------------

void CLCDriverMethods::QueueCreate(void *pObject, void **const ppQueue, const uint32 uiMaxLength, void (*pDestroyElement)(void *pElement))
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    pQueueLocal->RCreate(ppQueue, uiMaxLength, pDestroyElement);
}
void CLCDriverMethods::QueueDestroy(void *pObject, void **const ppQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    pQueueLocal->RDestroy(ppQueue);
}

ErrorCode_e CLCDriverMethods::QueueEnqueue(void *pObject, void *const ppQueue, void *const pValue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->REnqueue(ppQueue, pValue);
}

void *CLCDriverMethods::QueueDequeue(void *pObject, void *const ppQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RDequeue(ppQueue);
}

QueueCallback_fn CLCDriverMethods::QueueSetCallback(void *pObject, void *const pQueue, const QueueCallbackType_e Type, const QueueCallback_fn fnCallback, void *const pParam)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RSetCallback(pQueue, Type, fnCallback, pParam);
}

boolean CLCDriverMethods::QueueIsEmpty(void *pObject, const void *const pQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RIsEmpty(pQueue);
}

boolean CLCDriverMethods::QueueIsMember(void *pObject, const void *const pQueue, void *pValue, boolean(*Match)(void *pValue1, void *pValue2))
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RIsMember(pQueue, pValue, Match);
}

int CLCDriverMethods::QueueGetNrOfElements(void *pObject, const void *const pQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RGetNrOfElements(pQueue);
}

void CLCDriverMethods::RQueueCreate(void *pObject, void **const ppQueue, const uint32 uiMaxLength, void (*pDestroyElement)(void *pElement))
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    pQueueLocal->RCreate(ppQueue, uiMaxLength, pDestroyElement);
}

void CLCDriverMethods::RQueueDestroy(void *pObject, void **const ppQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    pQueueLocal->RDestroy(ppQueue);
}

ErrorCode_e CLCDriverMethods::RQueueEnqueue(void *pObject, void *const pQueue, void *const pValue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->REnqueue(pQueue, pValue);
}

void *CLCDriverMethods::RQueueDequeue(void *pObject, void *const pQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RDequeue(pQueue);
}

QueueCallback_fn CLCDriverMethods::RQueueSetCallback(void *pObject, void *const pQueue, const QueueCallbackType_e Type, const QueueCallback_fn fnCallback, void *const pParam)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RSetCallback(pQueue, Type, fnCallback, pParam);
}

boolean CLCDriverMethods::RQueueIsEmpty(void *pObject, const void *const pQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RIsEmpty(pQueue);
}

boolean CLCDriverMethods::RQueueIsMember(void *pObject, const void *const pQueue, void *pValue, boolean(*Match)(void *pValue1, void *pValue2))
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RIsMember(pQueue, pValue, Match);
}

int CLCDriverMethods::RQueueGetNrOfElements(void *pObject, const void *const pQueue)
{
    Queue *pQueueLocal = static_cast<Queue *>(pObject);
    return pQueueLocal->RGetNrOfElements(pQueue);
}

//----------------------------------------

ErrorCode_e CLCDriverMethods::CEHCallbackFunction(void *pObject, CommandData_t *pCmdData)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->m_pLoaderRpcFunctions->Do_CEH_Callback(pCmdData);
}

ErrorCode_e CLCDriverMethods::CEH_Z_CallbackFunction(void *pObject, CommandData_t *pCmdData)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->m_pZRpcFunctions->Do_CEH_Callback(pCmdData);
}

ErrorCode_e CLCDriverMethods::CEH_PROTROM_CallbackFunction(void *pObject, CommandData_t *pCmdData)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->m_pProtromRpcFunctions->Do_CEH_Callback(pCmdData);
}

ErrorCode_e CLCDriverMethods::CEH_A2_CallbackFunction(void *pObject, CommandData_t *pCmdData)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->m_pA2LoaderRpcFunctions->Do_CEH_Callback(pCmdData);
}

//----------------------------------------

void CLCDriverMethods::BulkCommandReqCallback(void *pObject, uint16 uiSession, uint32 uiChunkSize, uint64 uiOffset, uint32 uiLength, boolean bAckRead)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->m_pBulkHandler->HandleCommandRequest(uiSession, uiChunkSize, uiOffset, uiLength, bAckRead ? true : false);
}

void CLCDriverMethods::BulkDataReqCallback(void *pObject, uint16 uiSession, uint32 uiChunkSize, uint64 uiOffset, uint32 uiLength, uint64 uiTotalLength, uint32 uiTransferedLength)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->Do_BulkDataReqCallback(uiSession, uiChunkSize, uiOffset, uiLength, uiTotalLength, uiTransferedLength);
}

void CLCDriverMethods::BulkReleaseBuffers(void *pObject, TL_BulkVectorList_t *BulkVector_p)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->m_pBuffers->ReleaseBulkVector(BulkVector_p);
}


void CLCDriverMethods::BulkDataEndOfDumpCallback(void *pObject)
{
    CLCDriverMethods *pLcdMethods = static_cast<CLCDriverMethods *>(pObject);
    return pLcdMethods->Do_BulkDataEndOfDumpCallback();
}

////////////////////////////////////////////////////////////////////////
///
/// Exported methods
///

/// <summary>
/// Initializes LCDriver.
/// </summary>
/// <param name="pfnRead">Callback to external Read function.</param>
/// <param name="pfnWrite">Callback to external Write function.</param>
/// <param name="pfnCancel">Callback to external Cancel function.</param>
/// <param name="ppInstance">Method modifies value to point to this instance of m_pGlobalCommunicationLCDriver object.</param>
/// <param name="pfnMessage">Callback to external Message function for outputting text.</param>
/// <param name="pchLCMLibPath">Path to LCM module.</param>
/// <param name="pfnProgressBarCallback">Callback to external ProgressBar function.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Initialize(void **ppInstance)
{
#ifdef _MESSAGES
    m_pBuffers->SetLogger(m_pLogger);
    m_pQueue->SetLogger(m_pLogger);
    m_pTimer->SetLogger(m_pLogger);
    m_pSerialization->SetLogger(m_pLogger);
#endif
    int ReturnValue = E_SUCCESS;
    VERIFY(ppInstance != NULL, E_INVALID_INPUT_PARAMETERS);

    // Setup Command Protocol buffer
    m_pBuffers->Init();

    BuffersInterface_t BufferFunctions;
    TimersInterface_t TimerFunctions;
    QueueInterface_t QueueFunctions;

    // Setup timer struct
    VERIFY_SUCCESS(SetupTimers(&TimerFunctions));

    // Setup buffer struct
    VERIFY_SUCCESS(SetupBuffers(&BufferFunctions));

    // Setup queue struct
    VERIFY_SUCCESS(SetupQueues(&QueueFunctions));

    // Setup hash struct
    VERIFY_SUCCESS(SetupHash());

    if (NULL == m_pCommunicationDevice->Read || NULL == m_pCommunicationDevice->Write || NULL == m_pCommunicationDevice->Cancel) {
        VERIFY_SUCCESS(CALLBACKS_NOT_CONFIGURED_CORRECTLY);
    }

    VERIFY_SUCCESS(m_pLcmInterface->CommunicationInitialize(this, m_CurrentProtocolFamily, m_pHashDevice, m_pCommunicationDevice, m_CurrentCEHCallback, &BufferFunctions, &TimerFunctions, &QueueFunctions));
    *ppInstance = m_pCommunicationDevice;

    // Create main thread
    if (m_pTimer) {
        m_pMainThread = new CLCDriverThread(this);
    } else {
        VERIFY_SUCCESS(LCDRIVER_THREAD_NOT_STARTED);
    }

    // Initialize 32 timers for the moment (timer memory only allocated once for all instances)
    TimerInit(m_pTimer, 32);

    if (R15_FAMILY == m_CurrentProtocolFamily) {
        m_pMainThread->TimerOn();
    }

    //Now start LCDriver thread
    m_pMainThread->ResumeThread();

    m_pLogger->log("LCDriver/LCM started ok!");

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Set up Timers utility class required by LCM.
/// </summary>
/// <returns> Status of the command.</returns>
ErrorCode_e CLCDriverMethods::SetupTimers(TimersInterface_t *pTimerFunctions)
{
    ErrorCode_e Result = E_SUCCESS;
    pTimerFunctions->GetSystemTime_Fn = static_cast<GetSystemTime_t>(TimerGetSystemTime);
    pTimerFunctions->ReadTime_Fn = static_cast<ReadTime_t>(TimerReadTime);
    pTimerFunctions->TimerGet_Fn = static_cast<TimerGet_t>(TimerGet);
    pTimerFunctions->TimerRelease_Fn = static_cast<TimerRelease_t>(TimerRelease);
    pTimerFunctions->TimersInit_Fn = static_cast<TimersInit_t>(TimerInit);
    pTimerFunctions->Object_p = m_pTimer;
    return Result;
}

/// <summary>
/// Set up Buffer utility class required by LCM.
/// </summary>
/// <returns> Status of the command.</returns>
ErrorCode_e CLCDriverMethods::SetupBuffers(BuffersInterface_t *pBufferFunctions)
{
    ErrorCode_e Result = E_SUCCESS;
    pBufferFunctions->BufferAllocate_Fn = static_cast<BufferAllocate_t>(BufferAllocate);
    pBufferFunctions->BufferRelease_Fn = static_cast<BufferRelease_t>(BufferRelease);
    pBufferFunctions->BuffersAvailable_Fn = static_cast<BuffersAvailable_t>(BuffersAvailable);
    pBufferFunctions->BuffersDeinit_Fn = static_cast<BuffersDeinit_t>(BuffersDeinit);
    pBufferFunctions->BuffersInit_Fn = static_cast<BuffersInit_t>(BuffersInit);
    pBufferFunctions->Object_p = m_pBuffers;
    return Result;
}

/// <summary>
/// Set up Queue utility class required by LCM.
/// </summary>
/// <returns> Status of the command.</returns>
ErrorCode_e CLCDriverMethods::SetupQueues(QueueInterface_t *pQueueFunctions)
{
    ErrorCode_e Result = E_SUCCESS;
    pQueueFunctions->Fifo_GetNrOfElements_Fn = QueueGetNrOfElements;
    pQueueFunctions->Fifo_IsEmpty_Fn = QueueIsEmpty;
    pQueueFunctions->Fifo_IsMember_Fn = QueueIsMember;
    pQueueFunctions->Fifo_SetCallback_Fn = QueueSetCallback;
    pQueueFunctions->FifoCreate_Fn = QueueCreate;
    pQueueFunctions->FifoDequeue_Fn = QueueDequeue;
    pQueueFunctions->FifoDestroy_Fn = QueueDestroy;
    pQueueFunctions->FifoEnqueue_Fn = QueueEnqueue;
    pQueueFunctions->RFifo_GetNrOfElements_Fn = RQueueGetNrOfElements;
    pQueueFunctions->RFifo_IsEmpty_Fn = RQueueIsEmpty;
    pQueueFunctions->RFifo_IsMember_Fn = RQueueIsMember;
    pQueueFunctions->RFifo_SetCallback_Fn = RQueueSetCallback;
    pQueueFunctions->RFifoCreate_Fn = RQueueCreate;
    pQueueFunctions->RFifoDequeue_Fn = RQueueDequeue;
    pQueueFunctions->RFifoDestroy_Fn = RQueueDestroy;
    pQueueFunctions->RFifoEnqueue_Fn = RQueueEnqueue;
    pQueueFunctions->Object_p = m_pQueue;
    return Result;
}

/// <summary>
/// Set up Hash utility class required by LCM.
/// </summary>
/// <returns> Status of the command.</returns>
ErrorCode_e CLCDriverMethods::SetupHash()
{
    m_pHashDevice = new HashDevice_t;

    if (0 == m_pHashDevice) {
        return E_ALLOCATE_FAILED;
    }

    m_pHashDevice->Calculate = HashCalculate;
    m_pHashDevice->Cancel = HashCancel;
    m_pHashDevice->Object_p = m_pHash;
    return E_SUCCESS;
}

void CLCDriverMethods::CancelCurrentLoaderCommand()
{
    m_EventQueue.SignalEvent();
}

/// <summary>
/// The Loader Start-up Status command is sent by the ME to notify the host that it has started. The Status parameter indicates in what mode the loader started.
/// </summary>
/// <param name="pchVersion">Loader version identifier.</param>
/// <param name="piVersionSize">Number of allocated bytes for version string.</param>
/// <param name="pchProtocol">Protocol version identifier.</param>
/// <param name="piProtocolSize">Number of allocated bytes for protocol string.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Done_System_LoaderStartupStatus(char *pchVersion, int *piVersionSize, char *pchProtocol, int *piProtocolSize)
{
    int ReturnValue = E_SUCCESS;
    VERIFY((pchVersion != NULL && piVersionSize != NULL && pchProtocol != NULL && piProtocolSize != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(WaitForEvent(EVENT_CMD_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_LOADERSTARTUPSTATUS));
    VERIFY_SUCCESS(m_pCmdResult->System_LoaderStartupStatus_Status);

    //Copy output parameter from RPC class.
    CopyStringToArray(m_pCmdResult->System_LoaderStartupStatus_LoaderVersion, pchVersion, piVersionSize);
    CopyStringToArray(m_pCmdResult->System_LoaderStartupStatus_ProtocolVersion, pchProtocol, piProtocolSize);

    //TODO: enable LCM_LDR Version check here!!!!
    //LCM_t LCMType = LDR_LCM;
    //VERIFY_SUCCESS(m_pLcmInterface->CommunicationCheckVersion(pchProtocol,LCMType));

ErrorExit:

    if ((E_SUCCESS != ReturnValue) && (E_INVALID_INPUT_PARAMETERS != ReturnValue)) {
        *piVersionSize = 0;
        *piProtocolSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// The ChangeBaudrate command is used to instruct the loader to change the UART baud rate.
/// The ME does not return GeneralResponse to this command. It changes to new BaudRate
/// and expects the further communication over UART to be realized using the new BaudRate.
/// </summary>
/// <param name="Context">LCD context on which to execute the operation.</param>
/// <param name="iBaudRate">Indicates the new UART baud rate.</param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_System_ChangeBaudRate(int iBaudRate)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_ChangeBaudRate(uiSessionOut, iBaudRate));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The Reboot command is used to instruct the loader to reset the ME. Upon receiving this command,
/// the loader shuts down in a controlled fashion and restarts the ME. The Mode parameter is used to
/// select the mode of reset. The ME does not accept any further communication after a successful
/// response from this command has been returned.
/// </summary>
/// <param name="iMode">Indicates the mode of reset:
/// 0: indicates normal restart.
/// 1: indicates restart in service mode.
/// 2: indicates restart with JTAG debugging enabled.
/// 3: indicates restart in service mode and with JTAG debugging enabled.
/// </param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_System_Reboot(int iMode)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_Reboot(uiSessionOut, iMode));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The loader returns a list of implemented commands and whether they are permitted to
/// execute in the current loader state.
/// </summary>
/// <param name="pCmdList">A list with supported commands.</param>
/// <param name="piCmdListSize">Number of commands in CmdList.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_System_SupportedCommands(TSupportedCmd *pCmdList, int *piCmdListSize)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_SupportedCommands(uiSessionOut));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_SUPPORTEDCOMMANDS));

    //Copy output parameter from RPC class.
    *piCmdListSize = static_cast<int>(m_pCmdResult->System_SupportedCommands_CmdList.size());
    CopyVectorToArray(m_pCmdResult->System_SupportedCommands_CmdList, pCmdList, piCmdListSize);

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piCmdListSize = 0;
    }

    return ReturnValue;
}

int CLCDriverMethods::Do_System_CollectData(int iType, int *piSize, char *pData)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_CollectData(uiSessionOut, iType));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_COLLECTDATA));

    //Copy output parameter from RPC class.
    if (m_pCmdResult->System_CollectedData.length() > static_cast<size_t>(*piSize)) {
        m_pCmdResult->System_CollectedData.erase(*piSize - 1);
    }

    strcpy_s(pData, *piSize, m_pCmdResult->System_CollectedData.c_str());
    *piSize = m_pCmdResult->System_CollectedData.length();

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piSize = 0;
        *pData = '\0';
    }

    return ReturnValue;
}

/// <summary>
/// Receive, verify and execute software, which can be a signed loader.
/// After having sent this command, the ME attempts to read the software
/// payload data from the host using the Bulk protocol or from the flash
/// file system depending on the selected path. The current loader does not
/// respond to communication after this command has been successfully executed.
/// If the executed software is another loader, communication can be resumed once the
/// Loader Started command has been received. A response of E_OPERATION_CANCELLED
/// indicates the operation was canceled.
/// </summary>
/// <param name="pchDevicePath">Target path. If use bulk the path is on PC, else on ME.</param>
/// <param name="iUseBulk">If value = 1 -> source on PC, else source on ME.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_System_ExecuteSoftware(const uint32 ExecuteMode, const char *pchDevicePath, int iUseBulk)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    if (1 == ExecuteMode) {
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_ExecuteSoftware(uiSessionOut, ExecuteMode, pchDevicePath, 0));

    } else if (2 == ExecuteMode) {
        if (iUseBulk) {
            m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
            VERIFY_SUCCESS(m_pBuffers->AllocateBulkFile(pchDevicePath));
            uint64 uiLength = m_pBuffers->GetBulkFileLength();
            m_uiBulkLength = uiLength;
            m_uiBulkTransferred = 0;
            VERIFY_SUCCESS(m_pBulkHandler->Send(pchDevicePath));

            VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_ExecuteSoftware(uiSessionOut, ExecuteMode, BULK_PATH, uiLength));
            //VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_EXECUTESOFTWARE));
        } else {
            // TEMPORARY ONLY - QSJOMIK: CURRENTLY WE CANNOT KNOW THE SIZE OF FILE ON AN ME MEMORY CARD IF USEBULK=0 - DO WE NEED A PARAMETER??
            VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_ExecuteSoftware(uiSessionOut, ExecuteMode, pchDevicePath, 0));
            //VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_EXECUTESOFTWARE));
        }

        // reset session counters for this instance of the lcm
        m_pLcmInterface->CommandResetSessionCounters();
    } else {
        ReturnValue = INVALID_EXECUTION_MODE;
    }

ErrorExit:

    if (iUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);
        }

        m_pBuffers->ReleaseBulkFile();
    }

    return ReturnValue;
}

/// <summary>
/// This command is used to escalate the privileges of the operator. Two ways of
/// authentication are available by default; control key authentication and certificate based
/// authentication. The authentication command sets the loader in a specific
/// authentication context when it takes control over the command flow. After
/// receiving the authentication command, the loader sends the appropriate request for information to the PC.
/// </summary>
/// <param name="iType">Authentication type:0 = control key authentication,1 = certificate authentication.</param>
/// <param name="piSize">Size of puchData. Parameter can be 0</param>
/// <param name="puchdata">Data challenge. Parameter can be 0 </param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_System_Authenticate(int iType, int *piSize, unsigned char *puchdata)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_Authenticate(uiSessionOut, iType));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED | EVENT_CMD_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_AUTHENTICATE));

    if (0 != iType) {
        CopyVectorToArray(m_pCmdResult->System_AuthenticationChallenge_Buffer, puchdata, piSize);
    }

ErrorExit:

    if (0 != iType && E_SUCCESS != ReturnValue) {
        *piSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// This command deauthenticates the ME from prior
/// authentications to prevent unauthorized access.
/// </summary>
/// <param name="iType">Deauthentication type:0 = deauthenticate until next reboot of the ME,1 = permanent deauthentication.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_System_Deauthenticate(int iType)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_Deauthenticate(uiSessionOut, iType));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_DEAUTHENTICATE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used by the loader to retrieve the SimLock control keys from the host to authenticate a user.
/// The command is used in the authentication context.
/// </summary>
/// <param name="pSIMLockKeys">A struct with all lock/unlock keys.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Done_System_GetControlKeys(TSIMLockKeys *pSIMLockKeys)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((pSIMLockKeys->pchCLCKLock != NULL && pSIMLockKeys->pchCLCKUnLock != NULL && pSIMLockKeys->pchESLCKLock != NULL && pSIMLockKeys->pchESLCKUnLock != NULL && pSIMLockKeys->pchNLCKLock != NULL && pSIMLockKeys->pchNLCKUnLock != NULL && pSIMLockKeys->pchNSLCKLock != NULL && pSIMLockKeys->pchNSLCKUnLock != NULL && pSIMLockKeys->pchPCKLock != NULL && pSIMLockKeys->pchPCKUnLock != NULL && pSIMLockKeys->pchSPLCKLock != NULL && pSIMLockKeys->pchSPLCKUnLock != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoneRPC_System_GetControlKeys(uiSessionOut, E_SUCCESS,
                   pSIMLockKeys->pchNLCKLock, pSIMLockKeys->pchNSLCKLock,
                   pSIMLockKeys->pchSPLCKLock, pSIMLockKeys->pchCLCKLock,
                   pSIMLockKeys->pchPCKLock, pSIMLockKeys->pchESLCKLock,
                   pSIMLockKeys->pchNLCKUnLock, pSIMLockKeys->pchNSLCKUnLock,
                   pSIMLockKeys->pchSPLCKUnLock, pSIMLockKeys->pchCLCKUnLock,
                   pSIMLockKeys->pchPCKUnLock, pSIMLockKeys->pchESLCKUnLock));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_AUTHENTICATE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used by the loader to retrieve the SimLock control keys data buffer from the host to authenticate a user.
/// The command is used in the authentication context.
/// </summary>
/// <param name="SIMLockKeysData">SIMLockKeyData buffer that contain all SIMLock keys.</param>
/// <param name="iDataSize">SIMLockKeyData buffer size.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Done_System_GetControlKeysData(int iDataSize, unsigned char *pSIMLockKeysData)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(pSIMLockKeysData != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoneRPC_System_GetControlKeysData(uiSessionOut, E_SUCCESS,
                   iDataSize, pSIMLockKeysData));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_AUTHENTICATE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used by the loader to perform a certificate authentication.
/// The command is only used in the authentication context.
/// </summary>
/// <param name="puchChallengeData">Authentication challenge. This challenge must be signed
/// using the correct certificate and returned to the loader.</param>
/// <param name="iDataSize">Authentication challenge buffer length.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Done_System_AuthenticationChallenge(int iDataSize, unsigned char *puchChallengeData)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(puchChallengeData != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoneRPC_System_AuthenticationChallenge(uiSessionOut, E_SUCCESS, iDataSize, puchChallengeData));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_AUTHENTICATE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The Set System Time command is used to set the current epoch time for loader
/// to configure the real time clock and use it for file system operations.
/// </summary>
/// <param name="EpochTime">
/// Number of seconds that have elapsed since January 1, 1970
/// </param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_System_SetSystemTime(uint32 EpochTime)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_SetSystemTime(uiSessionOut, EpochTime));

    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_SETSYSTEMTIME));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to instruct the Loader to switch to a new communication device.
/// </summary>
/// <param name="Device">
/// Device ID of the communication device to switch to.
/// </param>
/// <param name="DeviceParam">
/// Communication device parameters.
/// </param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_System_SwitchCommunicationDevice(uint32 Device, uint32 DeviceParam)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_SwitchCommunicationDevice(uiSessionOut, Device, DeviceParam));

    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_SWITCHCOMMUNICATIONDEVICE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The Loader shuts down the global communication and enters in a Relay working mode.
/// </summary>
/// <param name="HostDeviceId">
/// Communication device number of the relay input (host device).
/// </param>
/// <param name="TargetDeviceId">
/// Communication device number of the relay ouptut (target device).
/// </param>
/// <param name="ControlDeviceId">
/// Communication device number for the loader commands (control device).
/// </param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_System_StartCommRelay(uint32 HostDeviceId, uint32 TargetDeviceId, uint32 ControlDeviceId)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_StartCommRelay(uiSessionOut, HostDeviceId, TargetDeviceId, ControlDeviceId));

    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SYSTEM, COMMAND_SYSTEM_STARTCOMMRELAY));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The loader shuts down in a controlled fashion and proceeds to shut down the ME itself.
/// The ME does not accept any further communication after a successful response from this
/// command has been returned.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_System_Shutdown()
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_System_ShutDown(uiSessionOut));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to initiate a flashing session. The type argument is
/// used to select the type of file to process and Length parameter
/// defines the total size of the file.
/// </summary>
/// <param name="pchPath">Target path. If iUseBulk = 1, path is on PC.</param>
/// <param name="pchType">Type of the opened file.</param>
/// <param name="iUseBulk">If to use bulk protocol. If target is on PC iUseBulk shall be 1.</param>
/// <param name="iDeleteBuffers">Specify whether to delete bulk buffers when finished. 1 -> delete buffers. 0 -> don't delete buffers.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Flash_ProcessFile(const char *pchPath, const char *pchType, int iUseBulk, int iDeleteBuffers)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((pchPath != NULL && pchType != NULL), E_INVALID_INPUT_PARAMETERS);
    VERIFY_SUCCESS(IsMainThreadAlive());

#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Do_Flash_ProcessFile - DeleteBuffers = %d", iDeleteBuffers);
#endif

    if (iUseBulk) {
        CLockCS lock(LCDMethodsCS);
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
        m_pLcmInterface->BulkBuffersRelease((void *)BulkReleaseBuffers);
        VERIFY_SUCCESS(m_pBuffers->AllocateBulkFile(pchPath));
        uint64 uiLength = m_pBuffers->GetBulkFileLength();
        m_uiBulkLength = uiLength;
        m_uiBulkTransferred = 0;
        VERIFY_SUCCESS(m_pBulkHandler->Send(pchPath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_ProcessFile(uiSessionOut, uiLength, pchType, BULK_PATH));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_PROCESSFILE));
    } else {
        // QSJOMIK: CURRENTLY WE CANNOT KNOW THE SIZE OF FILE ON AN ME MEMORY CARD IF USEBULK=0 - SEND 0 FOR NOW
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_ProcessFile(uiSessionOut, 0, pchType, pchPath));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_PROCESSFILE));
    }

ErrorExit:

    if (iUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);

            if (0 != iDeleteBuffers) {
                m_pBuffers->ReleaseBulkFile();
            }
        }
    }

    return ReturnValue;
}

/// <summary>
/// This command is used by the client application program to obtain the device tree.
/// </summary>
/// <param name="pDevices">A struct with information about one flash device.</param>
/// <param name="piDeviceSize">Number of flash devices, size of pDevices array.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Flash_ListDevices(TDevices *pDevices, int *piDeviceSize)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_ListDevices(uiSessionOut));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_LISTDEVICES));

    *piDeviceSize = DevicesNumber;

    for (int i = 0; i < *piDeviceSize; ++i) {
        pDevices[i].pchPath = Devices[i].Path_p;
        pDevices[i].pchType = Devices[i].Type_p;

        pDevices[i].iTypeSize = strlen(Devices[i].Type_p);
        pDevices[i].iPathSize = strlen(Devices[i].Path_p);

        pDevices[i].uiBlockSize = Devices[i].BlockSize;
        pDevices[i].uiStart = Devices[i].Start;
        pDevices[i].uiLength = Devices[i].Length;
    }

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piDeviceSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// This command is used to initiate a dump session.
/// </summary>
/// <param name="pchPathToDump">Path to the device to dump.</param>
/// <param name="uiStart">Start of the dump relative to the start of the device indicated by Path [Byte].</param>
/// <param name="uiLength">Length of the dump [Byte]. Actual length is determined by the device block size.</param>
/// <param name="pchFilePath">File path on PC to store dump data to.</param>
/// <param name="uiRedundantArea">Include redundant area in the dump.</param>
/// <param name="iUseBulk">1-> save dump data on PC, 0-> save dump data on ME.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Flash_DumpArea(const char *pchPathToDump, uint64 uiStart, uint64 uiLength, const char *pchFilePath, uint32 uiRedundantArea, int iUseBulk)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY((pchPathToDump != NULL && pchFilePath != NULL), E_INVALID_INPUT_PARAMETERS);
    VERIFY_SUCCESS(IsMainThreadAlive());

    if (iUseBulk) {
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
        m_uiBulkLength = uiLength;
        m_uiBulkTransferred = 0;
        VERIFY_SUCCESS(m_pBulkHandler->Receive(pchFilePath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_DumpArea(uiSessionOut, pchPathToDump, uiStart, uiLength, BULK_PATH, uiRedundantArea));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_DUMPAREA));
    } else {
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_DumpArea(uiSessionOut, pchPathToDump, uiStart, uiLength, pchFilePath, uiRedundantArea));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_DUMPAREA));
    }

ErrorExit:

    if (iUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);
        }
    }

    return ReturnValue;
}

/// <summary>
/// This command is used to erase a flash device or part of a flash device.
/// </summary>
/// <param name="pchPath">Path to the device to erase.</param>
/// <param name="uiStart">Start of the erase relative to the start of the device
/// indicated by path [Byte]. This must be a multiple of the block size of the device.</param>
/// <param name="uiLength">Length of the dump [Byte]. This must be a multiple of the block size of the device.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Flash_EraseArea(const char *pchPath, uint64 uiStart, uint64 uiLength)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(pchPath != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_EraseArea(uiSessionOut, pchPath, uiStart, uiLength));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_ERASEAREA));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to initiate a flashing of raw data. The type argument is
/// used to select the type of file to process and Length parameter
/// defines the total size of the file.
/// </summary>
/// <param name="Context">LCD context on which to execute the operation.</param>
/// <param name="pchPath">Target path. If iUseBulk = 1, path is on PC.</param>
/// <param name="uiStart">Address where flashing should start.</param>
/// <param name="uiLength">Length of data to be flashed.</param>
/// <param name="uiDevice">Device ID number.</param>
/// <param name="iUseBulk">If to use bulk protocol. If target is on PC iUseBulk shall be 1.</param>
/// <param name="iDeleteBuffers">Specify if to delete bulk buffers after finish. 1 -> delete buffers. 0 -> don't delete buffers.</param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_Flash_FlashRaw(const char *pchPath, uint64 uiStart, uint64 uiLength, uint32 uiDevice, int iUseBulk, int iDeleteBuffers)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Do_Flash_FlashRaw - DeleteBuffers = %d", iDeleteBuffers);
#endif

    if (iUseBulk) {
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
        VERIFY_SUCCESS(m_pBuffers->AllocateBulkFile(pchPath));

        uint64 uiFileLength = m_pBuffers->GetBulkFileLength();
        VERIFY((uiLength <= uiFileLength), E_INVALID_INPUT_PARAMETERS);

        m_uiBulkLength = uiLength;
        m_uiBulkTransferred = 0;
        VERIFY_SUCCESS(m_pBulkHandler->Send(pchPath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_FlashRaw(uiSessionOut, uiStart, uiLength, uiDevice, BULK_PATH));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_FLASHRAW));
    } else {
        // CURRENTLY WE CANNOT KNOW THE SIZE OF FILE ON AN ME MEMORY CARD IF USEBULK=0 - SEND 0 FOR NOW
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_FlashRaw(uiSessionOut, uiStart, 0, uiDevice, pchPath));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_FLASHRAW));
    }

ErrorExit:

    if (iUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);
        }

        if (0 != iDeleteBuffers) {
            m_pBuffers->ReleaseBulkFile();
        }
    }

    return ReturnValue;
}

/// <summary>
/// This command is used to set enhanced area in eMMC.
/// </summary>
/// <param name="Context">LCD context on which to execute the operation.</param>
/// <param name="pchPathToDump">Path to the device where area should be set.</param>
/// <param name="uiStart">Start of enhanced area [Byte].</param>
/// <param name="uiLength">Length of enhanced area [Byte].</param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_Flash_SetEnhancedArea(const char *pchPathToDump, uint64 uiStart, uint64 uiLength)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_SetEnhancedArea(uiSessionOut, pchPathToDump, uiStart, uiLength));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_SETENHANCEDAREA));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to select different options in the loader during its life cycle.
/// </summary>
/// <param name="Context">LCD context on which to execute the operation.</param>
/// <param name="uiProperty">Defines which property of the loader will be set.</param>
/// <param name="uiValue">The value with which the property will be set.</param>
/// <returns>Status of the command.</returns>>
int CLCDriverMethods::Do_Flash_SelectLoaderOptions(uint32 uiProperty, uint32 uiValue)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Flash_SelectLoaderOptions(uiSessionOut, uiProperty, uiValue));

    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FLASH, COMMAND_FLASH_SELECTLOADEROPTIONS));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command retrieves the properties of the specified file system volume. It is issued by the PC application.
/// </summary>
/// <param name="pchDevicePath">Path of file system volume.</param>
/// <param name="pchFSType">File system type.</param>
/// <param name="piFSTypeSize">Size of pchFSType array.</param>
/// <param name="puiSize">Total size of the file system [Byte].</param>
/// <param name="puiFree">Available space [Byte].</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_VolumeProperties(const char *pchDevicePath, char *pchFSType, int *piFSTypeSize, uint64 *puiSize, uint64 *puiFree)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_VolumeProperties(uiSessionOut, pchDevicePath));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_VOLUMEPROPERTIES));

    //Copy output parameter from RPC class.
    if (m_pCmdResult->FileSystem_VolumeProperties_FSType.length() > static_cast<size_t>(*piFSTypeSize)) {
        m_pCmdResult->FileSystem_VolumeProperties_FSType.erase(*piFSTypeSize - 1);
    }

    strcpy_s(pchFSType, *piFSTypeSize, m_pCmdResult->FileSystem_VolumeProperties_FSType.c_str());
    *piFSTypeSize = m_pCmdResult->FileSystem_VolumeProperties_FSType.length();
    *puiSize = m_pCmdResult->FileSystem_VolumeProperties_Size;
    *puiFree = m_pCmdResult->FileSystem_VolumeProperties_Free;

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *pchFSType = '\0';
        *piFSTypeSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// Formats unused file system volume specified in device path. This operation fails if the volume is currently in use.
/// </summary>
/// <param name="pchDevicePath">Device path of the file system volume.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_FormatVolume(const char *pchDevicePath)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_FormatVolume(uiSessionOut, pchDevicePath));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_FORMATVOLUME));

ErrorExit:
    return ReturnValue;
}


/// <summary>
/// List files and directories residing in specified path.
/// </summary>
/// <param name="pchPath">File system path.</param>
/// <param name="pEntries">Struct with file and directory information.</param>
/// <param name="piDeviceSize">Number of files or directories, number of element in pEntries array.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_ListDirectory(const char *pchPath, TEntries *pEntries, int *piDeviceSize)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_ListDirectory(uiSessionOut, pchPath));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_LISTDIRECTORY));

    //Copy output parameter from RPC class.
    *piDeviceSize = static_cast<int>(m_pCmdResult->FileSystem_ListDirectory_Entries.size());
    CopyVectorToArray(m_pCmdResult->FileSystem_ListDirectory_Entries, pEntries, piDeviceSize);

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piDeviceSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// This command moves a file from the source path to the destination path if the source and destination differ.
/// It also renames a file if the source path and the destination path are the same.
/// </summary>
/// <param name="pchSourcePath">File system path in ME to source.</param>
/// <param name="pchDestinationPath">File system path in ME to destination.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_MoveFile(const char *pchSourcePath, const char *pchDestinationPath)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_MoveFile(uiSessionOut, pchSourcePath, pchDestinationPath)); // DoRPC_FileSystem_MoveFile(&uiSessionOut, pchSourcePath, pchDestinationPath);
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_MOVEFILE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Delete the specified file or directory. The loader only deletes empty directories.
/// </summary>
/// <param name="pchTargetPath">File system path in ME.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_DeleteFile(const char *pchTargetPath)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(pchTargetPath != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_DeleteFile(uiSessionOut, pchTargetPath));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_DELETEFILE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to do the following: Copy a file from the PC to the ME. Copy a file between two directories or file systems on the ME.
/// Copy a file from the ME to the PC.
/// </summary>
/// <param name="pchSourcePath">If iSourceUseBulk = 1 -> path on PC, else path on ME.</param>
/// <param name="iSourceUseBulk">If = 1 -> source on PC, else source on ME.</param>
/// <param name="pchDestinationPath">If iDestinationUseBulk = 1 -> path on PC, else path on ME.</param>
/// <param name="iDestinationUseBulk">If = 1 -> destination on PC, else destination on ME.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_CopyFile(const char *pchSourcePath, int iSourceUseBulk, const char *pchDestinationPath, int iDestinationUseBulk)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((pchSourcePath != NULL && pchDestinationPath != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());

    if (iSourceUseBulk) { // PC -> ME
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
        VERIFY_SUCCESS(m_pBuffers->AllocateBulkFile(pchSourcePath));
        m_uiBulkLength = m_pBuffers->GetBulkFileLength();
        m_uiBulkTransferred = 0;
        VERIFY_SUCCESS(m_pBulkHandler->Send(pchSourcePath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_CopyFile(uiSessionOut, BULK_PATH, pchDestinationPath));

        //Wait for stat command
        VERIFY_SUCCESS(WaitForEvent(EVENT_CMD_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_PROPERTIES));

        struct stat fileInfo;
        stat(pchSourcePath, &fileInfo);
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoneRPC_File_System_Operations_Properties(uiSessionOut, E_SUCCESS, fileInfo.st_mode, fileInfo.st_size, (uint32)fileInfo.st_mtime, (uint32)fileInfo.st_atime, (uint32)fileInfo.st_ctime));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_COPYFILE));
    } else if (iDestinationUseBulk) { // ME -> PC
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
        VERIFY_SUCCESS(m_pBulkHandler->Receive(pchDestinationPath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_CopyFile(uiSessionOut, pchSourcePath, BULK_PATH));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_COPYFILE));
    } else {
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_CopyFile(uiSessionOut, pchSourcePath, pchDestinationPath));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_COPYFILE));
    }

ErrorExit:

    if (iSourceUseBulk || iDestinationUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);
        }

        if (iSourceUseBulk) {
            m_pBuffers->ReleaseBulkFile();
        }
    }

    return ReturnValue;
}

/// <summary>
/// This command is used to create a directory. It is issued by the PC application.
/// </summary>
/// <param name="pchTargetPath">File system path to target.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_CreateDirectory(const char *pchTargetPath)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_CreateDirectory(uiSessionOut, pchTargetPath));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_CREATEDIRECTORY));

ErrorExit:
    return ReturnValue;
}
/// <summary>
/// This command is used to retrieve the properties of a file or directory.
/// </summary>
/// <param name="pchTargetPath">File system path to target.</param>
/// <param name="puiMode">File type and access restrictions descriptor.</param>
/// <param name="puiSize">File size [Byte].</param>
/// <param name="piMTime">Last modification timestamp.</param>
/// <param name="piATime">Last access timestamp.</param>
/// <param name="piCTime">Creation timestamp.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_Properties(const char *pchTargetPath, uint32 *puiMode, uint64 *puiSize, int *piMTime, int *piATime, int *piCTime)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_Properties(uiSessionOut, pchTargetPath));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_PROPERTIES));

    //Copy output parameter from RPC class.
    *puiMode = m_pCmdResult->FileSystem_Properties_Mode;
    *puiSize = m_pCmdResult->FileSystem_Properties_Size;
    *piMTime = m_pCmdResult->FileSystem_Properties_MTime;
    *piATime = m_pCmdResult->FileSystem_Properties_ATime;
    *piCTime = m_pCmdResult->FileSystem_Properties_CTime;

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to change the access permissions of a specified path.
/// </summary>
/// <param name="pchTargetPath">File system path to target.</param>
/// <param name="iAccess">New access permissions.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_ChangeAccess(const char *pchTargetPath, int iAccess)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_ChangeAccess(uiSessionOut, pchTargetPath, iAccess));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_CHANGEACCESS));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to read manifests contained in load modules at the specified path and send the data back over bulk protocol.
/// </summary>
/// <param name="pchTargetPath">Path on PC to save manifest.</param>
/// <param name="pchSourcePath">Load module(s) file system path.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_FileSystem_ReadLoadModuleManifests(const char *pchTargetPath, const char *pchSourcePath)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pBulkHandler->Receive(pchTargetPath));
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_File_System_Operations_ReadLoadModulesManifests(uiSessionOut, pchSourcePath));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_FILE_SYSTEM_OPERATIONS, COMMAND_FILE_SYSTEM_OPERATIONS_READLOADMODULESMANIFESTS));

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        m_pBulkHandler->Finish(TRUE);
    } else {
        m_pBulkHandler->Finish(FALSE);
    }

    return ReturnValue;
}

/// <summary>
/// This command is used to read the specified bits from the OTP.
/// </summary>
/// <param name="iOtpId">OTP area ID.</param>
/// <param name="iBitStart">Starting offset [bit].</param>
/// <param name="iBitLength">Length of read [bit].</param>
/// <param name="puchDataBuffer">Received OTP data.</param>
/// <param name="piDataBufferSize">Size of puchDataBuffer.</param>
/// <param name="puchStatusBuffer">Lock status for each read bit.</param>
/// <param name="piStatusBufferSize">Size of puchStatusBuffer.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_OTP_ReadBits(int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer, int *piDataBufferSize , unsigned char *puchStatusBuffer, int *piStatusBufferSize)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((puchDataBuffer != NULL && piDataBufferSize != NULL && puchStatusBuffer != NULL && piStatusBufferSize != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_OTP_ReadBits(uiSessionOut, iOtpId, iBitStart, iBitLength));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_OTP, COMMAND_OTP_READBITS));

    //Copy output parameter from RPC class.
    CopyVectorToArray(m_pCmdResult->OTP_ReadBits_DataBuffer, puchDataBuffer, piDataBufferSize);
    CopyVectorToArray(m_pCmdResult->OTP_ReadBits_StatusBuffer, puchStatusBuffer, piStatusBufferSize);

    //clean up
    m_pCmdResult->OTP_ReadBits_DataBuffer.clear();
    m_pCmdResult->OTP_ReadBits_StatusBuffer.clear();

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piDataBufferSize = 0;
        *piStatusBufferSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// This command stores the specified bits in the loader internal OTP structures in RAM, it does not write to OTP.
/// </summary>
/// <param name="iOtpId">OTP area ID.</param>
/// <param name="iBitStart">Starting offset [bit].</param>
/// <param name="iBitLength">Length of write [bit].</param>
/// <param name="puchDataBuffer">OTP data to write.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_OTP_SetBits(int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    uint32 Bytes = 0;
    VERIFY(puchDataBuffer != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());

    Bytes = (iBitLength + 7) / 8;
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_OTP_SetBits(uiSessionOut, iOtpId, iBitStart, iBitLength, Bytes, puchDataBuffer));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_OTP, COMMAND_OTP_SETBITS));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command writes (burns) all data from loader internal OTP structures in RAM into the OTP fuse box.
/// </summary>
/// <param name="iOtpId">OTP area ID.</param>
/// <param name="iForceWrite">If true: write and lock all lockable areas even if not all bits are received in cache.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_OTP_WriteAndLock(int iOtpId, int iForceWrite)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_OTP_WriteAndLock(uiSessionOut, iOtpId, iForceWrite));

    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_OTP, COMMAND_OTP_WRITEANDLOCK));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command installs a secure object to the Boot record or OTP area.
/// </summary>
/// <param name="pchSourcePath">If iUseBulk = 1 -> path on PC, else path on ME.</param>
/// <param name="iDestination">Secure object destination address on ME.</param>
/// <param name="iUseBulk">Source on PC -> iUseBulk= 1. Source on ME -> iUseBulk= 0.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_OTP_StoreSecureObject(const char *pchSourcePath, int iDestination, int iUseBulk)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(pchSourcePath != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());

    if (iUseBulk) {
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);

        VERIFY_SUCCESS(m_pBuffers->AllocateBulkFile(pchSourcePath));
        m_uiBulkLength = m_pBuffers->GetBulkFileLength();
        m_uiBulkTransferred = 0;
        VERIFY_SUCCESS(m_pBulkHandler->Send(pchSourcePath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_OTP_StoreSecureObject(uiSessionOut, BULK_PATH, iDestination));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_OTP, COMMAND_OTP_STORESECUREOBJECT));
    } else {
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_OTP_StoreSecureObject(uiSessionOut, pchSourcePath, iDestination));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_OTP, COMMAND_OTP_STORESECUREOBJECT));
    }

ErrorExit:

    if (iUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);
        }

        m_pBuffers->ReleaseBulkFile();
    }

    return ReturnValue;
}

/// <summary>
/// Reads a specified unit from the global data storage area/partition specified by the DevicePath parameter.
/// </summary>
/// <param name="iGdfsId">GDFS ID.</param>
/// <param name="iUnit">Unit ID to read.</param>
/// <param name="puchDataBuffer">Received global data.</param>
/// <param name="piSize">Size of puchDataBuffer.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_ParameterStorage_ReadGlobalDataUnit(const char *pchGdfsId, int iUnit, unsigned char *puchDataBuffer, int *piSize)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((pchGdfsId != NULL && puchDataBuffer != NULL && piSize != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_ParameterStorage_ReadGlobalDataUnit(uiSessionOut, pchGdfsId, iUnit));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_PARAMETERSTORAGE, COMMAND_PARAMETERSTORAGE_READGLOBALDATAUNIT));

    //Copy output parameter from RPC class.
    CopyVectorToArray(m_pCmdResult->ParameterStorage_ReadGlobalDataUnit_DataBuffer, puchDataBuffer, piSize);

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// Writes a specified unit to the global data storage area/partition specified by the DevicePath parameter.
/// </summary>
/// <param name="iGdfsId">GDFS ID.</param>
/// <param name="iUnit">Unit ID to write.</param>
/// <param name="puchDataBuffer">Global data to write.</param>
/// <param name="iSize">Size of puchDataBuffer.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_ParameterStorage_WriteGlobalDataUnit(const char *pchGdfsId, int iUnit, const unsigned char *puchDataBuffer, int iSize)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((pchGdfsId != NULL && puchDataBuffer != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_ParameterStorage_WriteGlobalDataUnit(uiSessionOut, pchGdfsId, iUnit, iSize, (void *)puchDataBuffer));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_PARAMETERSTORAGE, COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATAUNIT));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Reads the complete global data storage area/partition specified by the DevicePath parameter (reads all units at once).
/// </summary>
/// <param name="iGdfsId">GDFS ID.</param>
/// <param name="pchPath">If iUseBulk=1 -> path on PC to save data set to. Else path on ME.</param>
/// <param name="iUseBulk">1-> save global data on PC, 0-> save global data on ME.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_ParameterStorage_ReadGlobalDataSet(const char *pchGdfsId, const char *pchPath, int iUseBulk)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((pchGdfsId != NULL && pchPath != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());

    if (iUseBulk) {
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
        VERIFY_SUCCESS(m_pBulkHandler->Receive(pchPath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_ParameterStorage_ReadGlobalDataSet(uiSessionOut, pchGdfsId, BULK_PATH));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_PARAMETERSTORAGE, COMMAND_PARAMETERSTORAGE_READGLOBALDATASET));
    } else {
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_ParameterStorage_ReadGlobalDataSet(uiSessionOut, pchGdfsId, pchPath));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_PARAMETERSTORAGE, COMMAND_PARAMETERSTORAGE_READGLOBALDATASET));
    }

ErrorExit:

    if (iUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);
        }
    }

    return ReturnValue;
}

/// <summary>
/// Writes the complete global data storage area/partition specified by the DevicePath parameter (writes all units at once).
/// </summary>
/// <param name="iGdfsId">GDFS ID.</param>
/// <param name="pchPath">f iUseBulk=1 -> path on PC. Else path on ME</param>
/// <param name="iUseBulk">1-> Global data source on PC, 0-> global data source on ME.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_ParameterStorage_WriteGlobalDataSet(const char *pchGdfsId, const char *pchPath, int iUseBulk)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((pchGdfsId != NULL && pchPath != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());

    if (iUseBulk) {
        VERIFY_SUCCESS(m_pBuffers->AllocateBulkFile(pchPath));
        uint64 uiLength = m_pBuffers->GetBulkFileLength();
        m_pLcmInterface->BulkSetCallbacks((void *)BulkCommandReqCallback, (void *)BulkDataReqCallback, (void *)BulkDataEndOfDumpCallback);
        m_uiBulkLength = uiLength;
        m_uiBulkTransferred = 0;
        VERIFY_SUCCESS(m_pBulkHandler->Send(pchPath));

        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_ParameterStorage_WriteGlobalDataSet(uiSessionOut, pchGdfsId, uiLength, BULK_PATH));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_PARAMETERSTORAGE, COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATASET));
    } else {
        VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_ParameterStorage_WriteGlobalDataSet(uiSessionOut, pchGdfsId, 0, pchPath));
        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_PARAMETERSTORAGE, COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATASET));
    }

ErrorExit:

    if (iUseBulk) {
        if (E_SUCCESS != ReturnValue) {
            m_pBulkHandler->Finish(TRUE);
        } else {
            m_pBulkHandler->Finish(FALSE);
        }

        m_pBuffers->ReleaseBulkFile();
    }

    return ReturnValue;
}

/// <summary>
/// Erases the complete global data storage area/partition specified by the DevicePath parameter.
/// </summary>
/// <param name="iGdfsId">GDFS ID.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_ParameterStorage_EraseGlobalDataSet(const char *pchGdfsId)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(pchGdfsId != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_ParameterStorage_EraseGlobalDataSet(uiSessionOut, pchGdfsId));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_PARAMETERSTORAGE, COMMAND_PARAMETERSTORAGE_ERASEGLOBALDATASET));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to set the ME domain.
/// </summary>
/// <param name="iDomain">Target domain.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Security_SetDomain(int iDomain)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Security_SetDomain(uiSessionOut, iDomain));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SECURITY, COMMAND_SECURITY_SETDOMAIN));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to get the ME domain.
/// </summary>
/// <param name="piWrittenDomain">The ME current domain.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Security_GetDomain(int *piWrittenDomain)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(piWrittenDomain != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Security_GetDomain(uiSessionOut));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SECURITY, COMMAND_SECURITY_GETDOMAIN));

    //Copy output parameter from RPC class.
    *piWrittenDomain = m_pCmdResult->Security_GetDomain_WrittenDomain;

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to read a security data unit (such as a secure static or dynamic data unit).
/// </summary>
/// <param name="iUnitId">Unit ID to read.</param>
/// <param name="piSize">Size of puchDataBuffer.</param>
/// <param name="puchDataBuffer">The unit data.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Security_GetProperties(int iUnitId, int *piSize, unsigned char *puchDataBuffer)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY((piSize != NULL && puchDataBuffer != NULL), E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Security_GetProperties(uiSessionOut, iUnitId));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SECURITY, COMMAND_SECURITY_GETPROPERTIES));

    //Copy output parameter from RPC class.
    CopyVectorToArray(m_pCmdResult->Security_GetProperties_DataBuffer, puchDataBuffer, piSize);

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// This command is used to write a security data unit (such as a secure static or dynamic data unit).
/// </summary>
/// <param name="iUnitId">Unit ID to write.</param>
/// <param name="iSize">Size of puchDataBuffer.</param>
/// <param name="puchDataBuffer">The data to write.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Security_SetProperties(int iUnitId, int iSize, const unsigned char *puchDataBuffer)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;
    VERIFY(puchDataBuffer != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Security_SetProperties(uiSessionOut, iUnitId, iSize, (void *)puchDataBuffer));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SECURITY, COMMAND_SECURITY_SETPROPERTIES));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command associates all security data units with the current ME.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Security_BindProperties()
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Security_BindProperties(uiSessionOut));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SECURITY, COMMAND_SECURITY_BINDPROPERTIES));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to initialize a SW version table, intended for checking the ARB functionality.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Security_InitARBTable(int iType, int iLength, const unsigned char *puarbdata)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Security_InitARBTable(uiSessionOut, iType, iLength, puarbdata));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SECURITY, COMMAND_SECURITY_INITARBTABLE));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// This command is used to invoke programming of RPMB authentication key.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Security_WriteRPMBKey(uint32 uiDev_id, uint32 uiCommercial)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pLoaderRpcFunctions->DoRPC_Security_WriteRPMBKey(uiSessionOut, uiDev_id,  uiCommercial));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_SECURITY, COMMAND_SECURITY_WRITERPMBKEY));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The A2 loader shuts down in a controlled fashion and proceeds to shut down the ME itself.
/// The ME does not accept any further communication after a successful response from this
/// command has been returned.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_A2_System_Shutdown()
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    m_pA2LoaderRpcFunctions->setTargetCpu(1);

    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_System_Shutdown(uiSessionOut));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The PC uses the Loader version command to request version information from the loader
/// The response holds the loader version information coded as ASCII characters in the data field
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_A2_System_LoaderVersion(char *pchLoaderVersion, int *piSize, int iTargetCPU)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    m_pA2LoaderRpcFunctions->setTargetCpu(iTargetCPU);
    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_System_LoaderVersion(uiSessionOut));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_SYSTEM, COMMAND_A2_SYSTEM_LOADERVERSION));

    CopyStringToArray(m_pCmdResult->A2_LoaderVersion, pchLoaderVersion, piSize);

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piSize = 0;
    }

    return ReturnValue;
}

/// <summary>
/// The Protocol version command is sent by the PC to request application protocol version from the loader.
/// The loader responds with a GR with the protocol version in the response field.
/// The version is coded as: 1 byte Major version + 1 byte Minor version.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_A2_System_Reset(int iTimeout)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    m_pA2LoaderRpcFunctions->setTargetCpu(1);
    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_System_Reset(uiSessionOut, iTimeout));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// The Loader on loader command is used to transfer a new loader to the ME
/// When the header or payload has been sent, the loader responds with a GR using status codes.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_A2_System_LoaderOnLoader(const char *pchPath, int iPLOffset, int iHLOffset, int iTargetCPU)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    MemMappedFile loaderFile;
    uint32 loaderSize;
    uint8 *loaderData = NULL;
    uint32 LeftToSend;
    size_t NumberOfPackets;
    const uint32 PacketSize = m_pCmdResult->A2_MaxLoaderPacketSize - 17;
    uint32 PayloadOffset;
    const void *temp;
    uint32 HL;
    uint32 HL_Real;
    uint32 PL;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(loaderFile.LoadFileData(pchPath));
    loaderSize = static_cast<uint32>(loaderFile.GetFileSize());
    loaderData = loaderFile.AllocateFileData(0, loaderSize);
    VERIFY(0 != loaderData, loaderFile.GetError());

    m_pA2LoaderRpcFunctions->setTargetCpu(iTargetCPU);

    // get header length.
    temp = loaderData + iHLOffset;
    HL = m_pSerialization->get_uint32_le(&temp);
    // pad to 8 byte (64 bit) alignments
    HL_Real = (HL + 7) & ~7;

    // calculate payload length
    PL = loaderSize - HL_Real;

    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_System_LoaderOnLoader(uiSessionOut, HL_Real, loaderData));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_SYSTEM, COMMAND_A2_SYSTEM_LOADERONLOADER));

    NumberOfPackets = PL / PacketSize;

    if (NumberOfPackets *PacketSize == PL) {
        NumberOfPackets--;
    }

    PayloadOffset = HL_Real;

    if (NumberOfPackets > 0) {
        m_pA2LoaderRpcFunctions->setMorePackets(1);

        for (size_t i = 0; i < NumberOfPackets; ++i) {
            VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_System_LoaderOnLoader(uiSessionOut, PacketSize, loaderData + PayloadOffset));
            PayloadOffset += PacketSize;
        }

        m_pA2LoaderRpcFunctions->setMorePackets(0);
    }

    // send last payload package
    LeftToSend = PL - PayloadOffset + HL_Real;
    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_System_LoaderOnLoader(uiSessionOut, LeftToSend, loaderData + PayloadOffset));

    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_SYSTEM, COMMAND_A2_SYSTEM_LOADERONLOADER));

ErrorExit:

    if (0 != loaderData) {
        loaderFile.ReleaseFileData(loaderData, 0, loaderSize);
    }

    return ReturnValue;
}

/// <summary>
/// The Program flash command is sent by the PC to write a block of data into the flash memory.
/// The block data is sent in the data field.
/// When a complete block has been transmitted the loader verifies the data and responds with a GR using status codes
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_A2_Flash_ProgramFlash(const char *pchPath, int iUseSpeedFlash)
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    MemMappedFile softwareFile;
    uint32 softwareSize;
    uint8 *softwareData = 0;
    const void *temp;
    uint32 headerSize;
    uint8 *payloadData;
    uint32 leftToSend;

    VERIFY_SUCCESS(IsMainThreadAlive());

    softwareFile.LoadFileData(pchPath);
    softwareSize = static_cast<uint32>(softwareFile.GetFileSize());
    softwareData = softwareFile.AllocateFileData(0, softwareSize);
    VERIFY(0 != softwareData, softwareFile.GetError());

    // A2 header length offset = 80
    temp = softwareData + 80;
    headerSize = Serialization::get_uint32_le(&temp);
    headerSize = (headerSize + 7) & ~7;

    payloadData = softwareData + headerSize;

    m_pA2LoaderRpcFunctions->setTargetCpu(2);
    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_Flash_VerifySignedHeader(uiSessionOut, headerSize, softwareData));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_FLASH, COMMAND_A2_FLASH_VERIFYSIGNEDHEADER));

    leftToSend = softwareSize - headerSize;

    if (iUseSpeedFlash) {
        temp = softwareData + 60; // start address is located at byte 60
        uint32 startAddress = Serialization::get_uint32_le(&temp);

        temp = softwareData + 28; // maximum block size is located at byte 28
        uint32 subBlockSize = Serialization::get_uint32_le(&temp);

        temp = softwareData + 64; // the software length (end address) is located at byte 64
        uint32 endAddress = Serialization::get_uint32_le(&temp);

        // speedflash command should be sent to ACC CPU
        m_pA2LoaderRpcFunctions->setTargetCpu(1);

        // start LCM speedflash mode
        m_pLcmInterface->A2SpeedflashStart();

        VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_Flash_Speedflash(uiSessionOut, startAddress, endAddress, subBlockSize));

        while (leftToSend) {
            Serialization::skip_uint32((const void **)&payloadData); //skip start address
            Serialization::skip_uint32((const void **)&payloadData); //skip block size
            leftToSend -= 8; // skip start address and block size in the image

            subBlockSize = min(subBlockSize, leftToSend);

            VERIFY_SUCCESS(WaitForEvent(EVENT_SPEEDFLASH, GROUP_A2_FLASH, COMMAND_A2_FLASH_SPEEDFLASH));

            if (0 == leftToSend - subBlockSize) {
                m_pLcmInterface->A2SpeedflashSetLastBlock();
            }

            VERIFY_SUCCESS(m_pLcmInterface->A2SpeedflashWriteBlock(payloadData, subBlockSize)); // send sub-block

            payloadData += subBlockSize;
            leftToSend -= subBlockSize;
        }

        VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_FLASH, COMMAND_A2_FLASH_SPEEDFLASH));
    } else {
        while (leftToSend) {
            uint32 startAddress = Serialization::get_uint32_le((const void **)&payloadData);
            uint32 blockSize = Serialization::get_uint32_le((const void **)&payloadData);
            leftToSend -= 8; // skip start address and block size in the image
            blockSize = min(blockSize, leftToSend);

            VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_Flash_SoftwareBlockAddress(uiSessionOut, startAddress, blockSize));
            VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_FLASH, COMMAND_A2_FLASH_SOFTWAREBLOCKADDRESS));

            VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_Flash_ProgramFlash(uiSessionOut, blockSize, payloadData));
            VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_FLASH, COMMAND_A2_FLASH_PROGRAMFLASH));

            payloadData += blockSize;
            leftToSend -= blockSize;
        }
    }

    // verify flash command should be sent to APP CPU
    m_pA2LoaderRpcFunctions->setTargetCpu(2);
    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_Flash_VerifySoftwareFlash(uiSessionOut));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_FLASH, COMMAND_A2_FLASH_VERIFYSOFTWAREFLASH));

ErrorExit:

    if (0 != softwareData) {
        softwareFile.ReleaseFileData(softwareData, 0, softwareSize);
    }

    return ReturnValue;
}

/// <summary>
/// The Erase flash command is used to erase the complete flash memory.
/// It checks what type of memory is used in the ME and erases the complete memory including the first memory block.
/// If more than one flash is attached, all of them will be erased
/// The loader responds with a GR using status codes
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_A2_Flash_EraseFlash()
{
    uint16 uiSessionOut = 0;
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    m_pA2LoaderRpcFunctions->setTargetCpu(2);
    VERIFY_SUCCESS(m_pA2LoaderRpcFunctions->DoRPC_A2_Flash_EraseFlash(uiSessionOut));
    VERIFY_SUCCESS(WaitForEvent(EVENT_GR_RECEIVED, GROUP_A2_FLASH, COMMAND_A2_FLASH_ERASEFLASH));

ErrorExit:
    return ReturnValue;
}

int CLCDriverMethods::Done_A2_Control_LoaderStarted()
{
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    VERIFY_SUCCESS(WaitForEvent(EVENT_CMD_RECEIVED, GROUP_A2_CONTROL, COMMAND_A2_CONTROL_LOADERSTARTED));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Switch protocol family.
/// </summary>
/// <param name="family">New protocol family to use.</param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_SwitchProtocolFamily(TFamily family)
{
    int ReturnValue = E_SUCCESS;
    char strMessage[100] = {0};

    VERIFY_SUCCESS(IsMainThreadAlive());

    m_pMainThread->TimerOff();

    switch (family) {
    case R15_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = R15_FAMILY;
        m_CurrentCEHCallback = static_cast<Do_CEH_Call_t>(CEHCallbackFunction);
        strcat_s(strMessage, "Do_SetProtocolFamily(R15_FAMILY)");
        m_pMainThread->TimerOn();
        break;

    case PROTROM_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = PROTROM_FAMILY;
        m_CurrentCEHCallback = static_cast<Do_CEH_Call_t>(CEH_PROTROM_CallbackFunction);
        strcat_s(strMessage, "Do_SwitchProtocolFamily(PROTROM_FAMILY)");
        break;

    case Z_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = Z_FAMILY;
        m_CurrentCEHCallback = static_cast<Do_CEH_Call_t>(CEH_Z_CallbackFunction);
        strcat_s(strMessage, "Do_SwitchProtocolFamily(Z_FAMILY)");
        m_pZRpcFunctions->Z_IndataBuffer->Clear();
        break;

    case A2_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = A2_FAMILY;
        m_CurrentCEHCallback = static_cast<Do_CEH_Call_t>(CEH_A2_CallbackFunction);
        strcat_s(strMessage, "Do_SwitchProtocolFamily(A2_FAMILY)");
        //m_pMainThread->TimerOn();
        break;

    default:
        return INVALID_INPUT_PARAMETERS;
    }

#ifdef _MESSAGES
    m_pLogger->log(strMessage);
#endif

    VERIFY_SUCCESS(m_pMainThread->SetLcmFamily(m_CurrentProtocolFamily, m_CurrentCEHCallback));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Set ME in service mode and receive chip id.
/// </summary>
/// <param name="puiChipId">Received chip id.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Z_SetInServiceMode(unsigned int *puiChipId)
{
    int ReturnValue = E_SUCCESS;

    time_t Start       = 0;
    time_t End         = 0;
    time_t ElapsedTime = 0;
    bool bEmptyBuffer  = false;
    unsigned char uchReceivedByte;
    unsigned char rguchReceivedChipId[8];
    int iCount = 0;

    VERIFY(puiChipId != NULL, E_INVALID_INPUT_PARAMETERS);
    VERIFY_SUCCESS(IsMainThreadAlive());

    //Receive a 'z' character.
    time(&Start);
    time(&End);
    ElapsedTime = End - Start;

    while (ElapsedTime < 30) {
        uchReceivedByte = m_pZRpcFunctions->Z_IndataBuffer->Pop(&bEmptyBuffer);

        if ((bEmptyBuffer == false) && (uchReceivedByte == 'z')) {
            break;
        }

        time(&End);
        ElapsedTime = End - Start;
        OS::Sleep(2);
    }

    if (ElapsedTime >= 30) {
        VERIFY_SUCCESS(TIMEOUT_NO_Z_DETECTED);
    }

    //Send version request.
    VERIFY_SUCCESS(m_pZRpcFunctions->DoRPC_Z_VersionRequest());

    //Receive chip id.
    time(&Start);
    time(&End);
    ElapsedTime = End - Start;

    while (ElapsedTime < 30) {
        uchReceivedByte = m_pZRpcFunctions->Z_IndataBuffer->Pop(&bEmptyBuffer);

        if (bEmptyBuffer == false) {
            rguchReceivedChipId[iCount++] = uchReceivedByte;

            if (iCount == 8) {
                break;
            }
        }

        time(&End);
        ElapsedTime = End - Start;

        OS::Sleep(2);
    }

    if (ElapsedTime >= 30) {
        VERIFY_SUCCESS(TIMEOUT_NO_CHIP_ID_DETECTED);
    }

    //Copy chip ID byte 1 and 2
    *puiChipId = rguchReceivedChipId[0] << 8;   //Major byte
    *puiChipId += rguchReceivedChipId[1];       //Minor byte

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Set communication baudrate when communicating via UART.
/// </summary>
/// <param name="iBaudrate">Baudrate to use. Valid values: 9600, 19200, 38400, 57600, 115200,
/// 230400, 460800, 921600, 1625000.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Z_SetBaudrate(int iBaudrate)
{
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());
    VERIFY_SUCCESS(m_pZRpcFunctions->DoRPC_Z_SetBaudrate(iBaudrate));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Exit Z-protocol and start using PROTROM-protocol.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_Z_Exit_Z_Protocol()
{
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    m_pCommunicationDevice->Cancel(m_pCommunicationDevice);

    //Send exit z-protocol command.
    VERIFY_SUCCESS(m_pZRpcFunctions->DoRPC_Z_Exit_Z_Protocol());

    //Change protocol family to PROTROM
    VERIFY_SUCCESS(Do_SwitchProtocolFamily(PROTROM_PROTOCOL_FAMILY));

    //Receive PROTROM command PDU: PROTROM_PDU_READY_TO_RECEIVE.
    VERIFY_SUCCESS(WaitForPROTROMResponseOrCancelOrTimeout(PROTROM_PDU_READY_TO_RECEIVE));

    //Send PROTROOM command PDU:PROTROM_PDU_RESULT.
    VERIFY_SUCCESS(m_pProtromRpcFunctions->DoRPC_PROTROM_ResultPdu(0));

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Download loader using PROTROM-protocol.
/// </summary>
/// <param name="pchPath">Path to loader on PC.</param>
/// <param name="iPLOffset">Offset in header to payload length.</param>
/// <param name="iHLOffset">Offset in header to header length.</param>
/// <param name="iContinueProtRom">1 -> continue use PROTROM-protocol after download the loader. O -> not use PROTROM-protocol after download the loader.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::Do_PROTROM_DownloadLoader(const char *pchPath, int iPLOffset, int iHLOffset, int iContinueProtRom)
{
    int ReturnValue = E_SUCCESS;

    MemMappedFile loaderFile;
    uint8 *loaderData;
    const void *temp;
    uint32 HL;                    //Header length.
    uint32 HL_Real;               //Header length + 32 bit alignment padding
    uint32 PL;                    //Payload length.
    const uint32 PacketSize = 64000;
    size_t NumberOfPackets;
    uint32 PayloadOffset;
    uint32 LeftToSend;
    uint8 *Header_p;
    uint32 HeaderIdentifier           = 0;

    VERIFY(pchPath != NULL, E_INVALID_INPUT_PARAMETERS);

    VERIFY_SUCCESS(IsMainThreadAlive());

    VERIFY_SUCCESS(loaderFile.LoadFileData(pchPath));
    /* coverity[tainted_data_return] */
    loaderData = loaderFile.AllocateFileData(0, loaderFile.GetFileSize());
    VERIFY(0 != loaderData, loaderFile.GetError());

    Header_p = loaderData;
    // Only version A2 type headers is supported. This is identfied with
    // HEADER_A2_IDENTIFIER.
    HeaderIdentifier = (Header_p[0] | (Header_p[1] << 8) | (Header_p[2] << 16) | (Header_p[3] << 24));

    VERIFY((HeaderIdentifier == HEADER_A2_IDENTIFIER), E_LOADER_SEC_LIB_HEADER_VERIFICATION_FAILURE);

    //Get header length.
    temp = loaderData + iHLOffset;
    HL = m_pSerialization->get_uint32_le(&temp);
    HL_Real = (HL + 3) & ~3;                                    //Pad to 4 byte (32 bit) alignments

    //Get payload length.
    temp = loaderData + iPLOffset;
    PL = m_pSerialization->get_uint32_le(&temp);

    VERIFY(((PL + HL_Real) == loaderFile.GetFileSize()), E_LOADER_SEC_LIB_HEADER_VERIFICATION_FAILURE);

    VERIFY_SUCCESS(m_pProtromRpcFunctions->DoRPC_PROTROM_SendLoaderHeader(loaderData, HL_Real));

    //Receive PROTROM command PDU: PROTROM_PDU_RESULT.
    VERIFY_SUCCESS(WaitForPROTROMResponseOrCancelOrTimeout(PROTROM_PDU_RESULT));

    //Download all packet beside one
    NumberOfPackets = PL / PacketSize;

    if (NumberOfPackets *PacketSize == PL) {
        NumberOfPackets--;
    }

    PayloadOffset = HL_Real;

    if (NumberOfPackets > 0) {
        for (size_t i = 0; i < NumberOfPackets; ++i) {
            VERIFY_SUCCESS(m_pProtromRpcFunctions->DoRPC_PROTROM_SendLoaderPayload(loaderData + PayloadOffset, PacketSize));
            PayloadOffset += PacketSize;

            //Receive PROTROM command PDU: PROTROM_PDU_RESULT.
            VERIFY_SUCCESS(WaitForPROTROMResponseOrCancelOrTimeout(PROTROM_PDU_RESULT));
        }
    }

    if (!iContinueProtRom) {
        m_pLcmInterface->CommunicationCancelReceiver(2);
    }

    //Send last payload package
    LeftToSend = PL - PayloadOffset + HL_Real;

    VERIFY_SUCCESS(m_pProtromRpcFunctions->DoRPC_PROTROM_SendLoaderFinalPayload(loaderData + PayloadOffset, LeftToSend));

    loaderFile.ReleaseFileData(loaderData, 0, loaderFile.GetFileSize());

    //If downloaded loader continues to use PROTROM protocol.
    if (0 != iContinueProtRom) {
        //Receive PROTROM command PDU: PROTROM_PDU_RESULT.
        VERIFY_SUCCESS(WaitForPROTROMResponseOrCancelOrTimeout(PROTROM_PDU_RESULT));

        //Receive PROTROM command PDU: PROTROM_PDU_READY_TO_RECEIVE.
        VERIFY_SUCCESS(WaitForPROTROMResponseOrCancelOrTimeout(PROTROM_PDU_READY_TO_RECEIVE));

        //Send result pdu.
        VERIFY_SUCCESS(m_pProtromRpcFunctions->DoRPC_PROTROM_ResultPdu(0));
    } else {
        VERIFY_SUCCESS(WaitForPROTROMResponseOrCancelOrTimeout(PROTROM_PDU_RESULT));
    }

ErrorExit:
    return ReturnValue;
}

/// <summary>
/// Send intrinsic request to read security data using PROTROM-protocol.
/// </summary>
/// <param name="uiSecDataId">ID of the security data to be read with the intrinsic request.</param>
/// <param name="puchDataBuffer">Buffer where the read data will be stored.</param>
/// <param name="piDataLength">IN: Length of DataBuffer; OUT: Read data length.</param>
/// <returns>Status of the command.</returns>
int CLCDriverMethods::Do_PROTROM_ReadSecurityData(uint8 uiSecDataId, unsigned char *puchDataBuffer, int *piDataLength)
{
    int ReturnValue = E_SUCCESS;

    VERIFY_SUCCESS(IsMainThreadAlive());

    VERIFY_SUCCESS(m_pProtromRpcFunctions->DoRPC_PROTROM_ReadSecurityData(uiSecDataId));

    //Receive PROTROM command PDU: PROTROM_PDU_RESULT.
    VERIFY_SUCCESS(WaitForPROTROMResponseOrCancelOrTimeout(PROTROM_PDU_SECURITY_DATA_RES));

    CopyVectorToArray(m_pCmdResult->ProtromPayloadData, puchDataBuffer, piDataLength);

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        *piDataLength = 0;
    }

    return ReturnValue;
}

int CLCDriverMethods::SetInitialProtocolFamily(TFamily family)
{
    switch (family) {
    case R15_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = R15_FAMILY;
        m_CurrentCEHCallback = CEHCallbackFunction;
        break;

    case PROTROM_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = PROTROM_FAMILY;
        m_CurrentCEHCallback = CEH_PROTROM_CallbackFunction;
        break;

    case Z_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = Z_FAMILY;
        m_CurrentCEHCallback = CEH_Z_CallbackFunction;
        break;

    case A2_PROTOCOL_FAMILY:
        m_CurrentProtocolFamily = A2_FAMILY;
        m_CurrentCEHCallback = CEH_A2_CallbackFunction;
        break;

    default:
        return INVALID_INPUT_PARAMETERS;
    }

    return 0;
}

int CLCDriverMethods::ConfigureCommunicationDevice(void *Read_fn, void *Write_fn, void *Cancel_fn)
{
    int ReturnValue = E_SUCCESS;
    VERIFY((Read_fn != NULL && Write_fn != NULL && Cancel_fn != NULL), E_INVALID_INPUT_PARAMETERS);
    m_pCommunicationDevice->Read = reinterpret_cast<DeviceRead_fn>(Read_fn);
    m_pCommunicationDevice->Write = reinterpret_cast<DeviceWrite_fn>(Write_fn);
    m_pCommunicationDevice->Cancel = reinterpret_cast<DeviceCancel_fn>(Cancel_fn);
    m_pCommunicationDevice->Context_p = NULL;

ErrorExit:
    return ReturnValue;
}

int CLCDriverMethods::SetMessageCallback(void *Callback_fn)
{
    if (Callback_fn != NULL) {
        m_pLogger->setMessageCallback(reinterpret_cast<MessageCallback_t>(Callback_fn));
        return 0;
    } else {
        return E_INVALID_INPUT_PARAMETERS;
    }
}

int CLCDriverMethods::SetProgressCallback(void *Callback_fn)
{
    if (Callback_fn != NULL) {
        m_ProgressBarUpdate = reinterpret_cast<ProgressBarCallback_t>(Callback_fn);
        return 0;
    } else {
        return E_INVALID_INPUT_PARAMETERS;
    }
}

/// <summary>
/// Set timeouts used by LCM and LCDriver.
/// </summary>
/// <param name="R15_TOs">LCM timeout data.</param>
/// <param name="LCD_TOs">LCDriver timeout data.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::SetPcTimeouts(TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs)
{
    CLockCS TimeoutLock(m_SetTimeoutCS);

    // if null, default values are used
    if (LCD_TOs != NULL) {
        m_Timeouts.uiRTO = LCD_TOs->uiRTO;
        m_Timeouts.uiSTO = LCD_TOs->uiSTO;
    }

    if (m_CurrentProtocolFamily == R15_FAMILY) {
        LcmR15Timeouts timeouts;
        timeouts.TCACK = R15_TOs->TCACK;
        timeouts.TBCR = R15_TOs->TBCR;
        timeouts.TBDR = R15_TOs->TBDR;
        return m_pLcmInterface->CommunicationSetProtocolTimeouts(static_cast<void *>(&timeouts));
    } else {
        return E_SUCCESS;
    }
}

/// <summary>
/// Get timeouts used by LCM and LCDriver.
/// </summary>
/// <param name="R15_TOs">LCM timeout data.</param>
/// <param name="LCD_TOs">LCDriver timeout data.</param>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::GetPcTimeouts(TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs)
{
    int ReturnValue = E_SUCCESS;
    LCD_TOs->uiRTO = m_Timeouts.uiRTO;
    LCD_TOs->uiSTO = m_Timeouts.uiSTO;

    if (m_CurrentProtocolFamily == R15_FAMILY) {
        LcmR15Timeouts timeouts;
        ReturnValue = m_pLcmInterface->CommunicationGetProtocolTimeouts(static_cast<void *>(&timeouts));

        if (E_SUCCESS == ReturnValue) {
            R15_TOs->TCACK = timeouts.TCACK;
            R15_TOs->TBCR = timeouts.TBCR;
            R15_TOs->TBDR = timeouts.TBDR;
            R15_TOs->TBES = 0xFFFFFFFF;
        }
    } else {
        R15_TOs->TBCR = 0;
        R15_TOs->TBDR = 0;
        R15_TOs->TBES = 0;
        R15_TOs->TCACK = 0;
    }

    return ReturnValue;
}

/// <summary>
/// Copies a vector to an array.
/// </summary>
/// <param name="Source">Source data.</param>
/// <param name="pDestination">The destination to write data to.</param>
/// <param name="piSize">Size of data to write.</param>
/// <returns> Void.</returns>
template <class T, class U>
void CLCDriverMethods::CopyVectorToArray(const vector<T> &Source, U *pDestination, int *piSize)
{
    size_t copyLength = *piSize;

    if (Source.size() < copyLength) {
        copyLength = Source.size();
    }

    for (size_t i = 0; i < copyLength; i++) {
        pDestination[i] = Source[i];
    }

    *piSize = static_cast<int>(copyLength);
}

void CLCDriverMethods::CopyStringToArray(string &Source, char *pDestination, int *piSize)
{
    if (Source.size() > static_cast<size_t>(*piSize)) {
        // erase part of the string that won't fit in the buffer
        Source.erase(*piSize - 1);
    }

    strcpy_s(pDestination, *piSize, Source.c_str());
    *piSize = Source.size();
}

/// <summary>
/// Check whether main thread is alive and perform other checks.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::IsMainThreadAlive()
{
    if (0 != m_pMainThread->ThreadIsDying()) {
        if (m_pMainThread->ThreadIsDying() == 2) {
            return LCDRIVER_THREAD_KILLED_WITH_SIGNAL_DEATH;
        } else if (m_pMainThread->ThreadIsDying() == 3) {
            return LCDRIVER_THREAD_KILLED_WITH_CANCEL;
        } else {
            return LCDRIVER_THREAD_KILLED;
        }
    }

    return 0;
}

/// <summary>
/// Wait for General Response from ME.
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::WaitForEvent(uint32 event, int Group, int Command)
{
    int iResult;
    Event *receivedEvent;
    CLockCS TimeoutLock(m_SetTimeoutCS);

    m_pLogger->log("WaitForEvent: Entering method. Event = 0x%08x, Timeout = %d", event, m_Timeouts.uiRTO);
    RemoveResult result = m_EventQueue.RemoveHead(reinterpret_cast<void **>(&receivedEvent), m_Timeouts.uiRTO);

    // always wait for error event
    event |= EVENT_ERROR;

    if (REMOVE_SUCCESS == result) {
        switch (receivedEvent->event & event) {
        case EVENT_GR_RECEIVED:

            if (receivedEvent->group == Group &&
                    receivedEvent->command == Command) {
                m_pLogger->log("WaitForEvent: General Response received. Number = %d", receivedEvent->command);
                iResult = receivedEvent->error;
            } else {
                m_pLogger->log("Expected GR command # %d", Command);
                m_pLogger->log("Expected GR application (group) # %d", Group);
                m_pLogger->log("Received GR command # %d", receivedEvent->command);
                m_pLogger->log("Received GR application (group) # %d", receivedEvent->group);
                iResult = GENERAL_RESPONSE_COMMAND_NUMBER_ERROR;
            }

            break;

        case EVENT_CMD_RECEIVED:

            if ((receivedEvent->group == Group && receivedEvent->command == Command) ||
                    (Group == GROUP_SYSTEM && Command == COMMAND_SYSTEM_AUTHENTICATE)) {
                m_pLogger->log("WaitForEvent: Expected command received");
                iResult = 0;
            } else {
                m_pLogger->log("WaitForEvent: ERROR Unexpected command received");
                iResult = COMMAND_NUMBER_ERROR;
            }

            break;

        case EVENT_SPEEDFLASH:
            m_pLogger->log("WaitForEvent: Speedflash request received.");
            iResult = 0;
            break;

        case EVENT_ERROR:
            iResult = receivedEvent->error;
            break;

        default:
            iResult = UNEXPECTED_EVENT_RECEIVED;
            break;
        }
    } else if (REMOVE_CANCEL == result) {
        m_pLogger->log("WaitForEvent: Cancel received");
        iResult = CANCEL_EVENT_RECEIVED;
    } else if (REMOVE_TIMEOUT == result) {
        m_pLogger->log("WaitForEvent: Timeout");
        iResult = TIMEOUT;
    } else if (REMOVE_WAIT_FOR_SEMAPHORE == result) {
        m_pLogger->log("WaitForEvent: Waiting for semaphore signal");
        iResult = 0;
    } else {
        m_pLogger->log("WaitForEvent: Unknown return value");
        iResult = UNKNOWN_WAIT_RETURN_VALUE;
    }

    return iResult;
}

/// <summary>
/// Wait for specific Protrom response.
/// <param name="iReceivePdu">Which PDU to wait for.</param>
/// </summary>
/// <returns> Status of the command.</returns>
int CLCDriverMethods::WaitForPROTROMResponseOrCancelOrTimeout(int iReceivePdu)
{
    int iResult;
    bool Continue = false;
    TProtromInfo *pInfo = 0;
    CLockCS TimeoutLock(m_SetTimeoutCS);

    do {
        Continue = false;
        m_pLogger->log("WaitForPROTROMResponseOrCancelOrTimeout: WAIT - expected PDU = %d", iReceivePdu);

        RemoveResult result = m_pProtromRpcFunctions->ProtromQueue.RemoveRequest(reinterpret_cast<void **>(&pInfo), m_Timeouts.uiRTO);

        if (REMOVE_SUCCESS == result) {
            iResult = 0;

            if (pInfo->ReceivedPdu != iReceivePdu) {
                m_pLogger->log("WaitForPROTROMResponseOrCancelOrTimeout: RECEIVE - unexpected PDU %d", pInfo->ReceivedPdu);
                Continue = true;
            } else if (iReceivePdu == PROTROM_PDU_RESULT) {
                //Check result from PDU_RESULT
                if (pInfo->Status != 0) {
                    iResult = PROTROM_STATUS_NOT_OK;
                }
            }

            delete[](uint8 *)pInfo->DataP;
            delete pInfo;
        } else if (REMOVE_CANCEL == result) {
            m_pLogger->log("WaitForPROTROMResponseOrCancelOrTimeout: CANCEL");
            iResult = CANCEL_EVENT_RECEIVED;
        } else if (REMOVE_TIMEOUT == result) {
            m_pLogger->log("WaitForPROTROMResponseOrCancelOrTimeout: TIMEOUT");
            iResult = TIMEOUT;
        } else {
            m_pLogger->log("WaitForPROTROMResponseOrCancelOrTimeout: UNKNOWN");
            iResult = UNKNOWN_WAIT_RETURN_VALUE;
        }
    } while (Continue);

    return iResult;
}

void CLCDriverMethods::Do_BulkDataReqCallback(uint16 Session, uint32 ChunkSize, uint64 Offset, uint32 Length, uint64 TotalLength, uint32 TransferredLength)
{
    m_uiBulkTransferred += ChunkSize;
}

void CLCDriverMethods::UpdateBulkProgress()
{
    if (NULL != m_ProgressBarUpdate) {
        m_ProgressBarUpdate(m_pCommunicationDevice, m_uiBulkLength, m_uiBulkTransferred);
    }
}

void CLCDriverMethods::Do_BulkDataEndOfDumpCallback()
{
#ifdef _BULKDEBUG
    m_pLogger->log("BULK: END OF DUMP CALLBACK RECEIVED!");
#endif
}

int CLCDriverMethods::MapLcmError(int error)
{
    switch (error) {
    case E_RETRANSMITION_FAILED:
        return LCM_RETRANSMISSION_ERROR;

    case E_GENERAL_COMMUNICATION_ERROR:
        return LCM_DEVICE_WRITE_ERROR;

    default:
        return error;
    }
}

void CLCDriverMethods::SignalError(int error)
{
    Event *event = new Event(EVENT_ERROR, error);
    m_EventQueue.AddTail(event);
}

void CLCDriverMethods::AddEvent(Event *newEvent)
{
    m_EventQueue.AddTail(newEvent);
}
