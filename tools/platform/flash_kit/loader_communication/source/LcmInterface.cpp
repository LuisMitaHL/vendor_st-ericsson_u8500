/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include "lcdriver_error_codes.h"
#include "LcmInterface.h"
#include "Error.h"
#include <string.h>
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#include <dlfcn.h>
#define GetProcAddress dlsym
#endif

char *LcmInterface::m_pchLCMLibPath = 0;
#if !(defined(__MINGW32__) || defined(__linux__) || defined(__APPLE__))
char LCD_LCM_Compatibility[] = "TestBuild"; // Generate Test String for unofficial builds in Windows environment
#endif // __MINGW32__ || __linux__ || __APPLE__
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//***********************************************************************************************
// Name:    LcmInterface()
// Desc:    Constructor.
//**********************************************************************************************/
LcmInterface::LcmInterface() : m_pCommunication(NULL) , lcmError_(E_SUCCESS)
{
}

//***********************************************************************************************
// Name:    ~LcmInterface()
// Desc:    Destructor.
//**********************************************************************************************/
LcmInterface::~LcmInterface()
{
}

HMODULE                  LcmInterface::m_hDLL = 0;
R15BulkInterface_t       LcmInterface::R15Bulk;
R15CommandInterface_t    LcmInterface::R15Command;
A2CommandInterface_t     LcmInterface::A2Command;
CommunicationInterface_t LcmInterface::Communication;
CCriticalSectionObject   LcmInterface::m_CriticalSection;

//***********************************************************************************************
// Name:    RunOnce()
// Desc:    Load LCM module and initialize function pointers once only.
//**********************************************************************************************/
int LcmInterface::LoadLCMLibrary()
{
    if (m_pchLCMLibPath == NULL) {
#ifdef _WIN32
        m_hDLL = LoadLibrary("LCM");
#else
        m_hDLL = dlopen("./lcm_linux/liblcm.so.1", RTLD_LAZY);
#endif
    } else {
#ifdef _WIN32
        m_hDLL = LoadLibrary(m_pchLCMLibPath);
#else
        m_hDLL = dlopen(m_pchLCMLibPath, RTLD_LAZY);
#endif
    }

    if (m_hDLL == NULL) {
        return LCM_DLL_LOAD_LOADLIBRARY_ERROR;
    }

    // Link to all necessary methods in the LCM DLL
    Communication.Initialize_Fn             = (CommunicationInitialize_t)GetProcAddress(m_hDLL, "Do_Communication_Initialize");
    Communication.Shutdown_Fn               = (CommunicationShutdown_t)GetProcAddress(m_hDLL, "Do_Communication_Shutdown");
    Communication.Poll_Fn                   = (CommunicationPoll_t)GetProcAddress(m_hDLL, "Do_Communication_Poll");
    Communication.SetFamily_Fn              = (CommunicationSetFamily_t)GetProcAddress(m_hDLL, "Do_Communication_SetFamily");
    Communication.Send_Fn                   = (CommunicationSend_t)GetProcAddress(m_hDLL, "Do_Communication_Send");
    Communication.SetProtocolTimeouts_Fn    = (CommunicationSetProtocolTimeouts_t)GetProcAddress(m_hDLL, "Do_Communication_SetProtocolTimeouts");
    Communication.GetProtocolTimeouts_Fn    = (CommunicationGetProtocolTimeouts_t)GetProcAddress(m_hDLL, "Do_Communication_GetProtocolTimeouts");
    Communication.GetVersion_Fn             = (CommunicationGetVersion_t)GetProcAddress(m_hDLL, "Do_Communication_GetVersion");
    Communication.CancelReceiver_Fn         = (CommunicationCancelReceiver_t)GetProcAddress(m_hDLL, "Do_Communication_Cancel_Receiver");

    R15Command.Send_Fn                  = (R15CommandSend_t)GetProcAddress(m_hDLL, "Do_R15_Command_Send");
    R15Command.ResetSessionCounters_Fn  = (R15CommandResetSessionCounters_t)GetProcAddress(m_hDLL, "Do_R15_Command_ResetSessionCounters");

    R15Bulk.OpenSession_Fn              = (R15BulkOpenSession_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_OpenSession");
    R15Bulk.CreateVector_Fn             = (R15BulkCreateVector_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_CreateVector");
    R15Bulk.DestroyVector_Fn            = (R15BulkDestroyVector_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_DestroyVector");
    R15Bulk.StartSession_Fn             = (R15BulkStartSession_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_StartSession");
    R15Bulk.GetStatusSession_Fn         = (R15BulkGetStatusSession_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_GetStatusSession");
    R15Bulk.CloseSession_Fn             = (R15BulkCloseSession_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_CloseSession");
    R15Bulk.SetCallbacks_Fn             = (R15BulkSetCallbacks_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_SetCallbacks");
    R15Bulk.SetBuffersRelease_Fn        = (R15BulkBuffersRelease_t)GetProcAddress(m_hDLL, "Do_R15_Bulk_SetBuffersRelease");

    A2Command.Send_Fn                       = (A2CommandSend_t)GetProcAddress(m_hDLL, "Do_A2_Command_Send");
    A2Command.SpeedflashStart_Fn            = (A2SpeedflashStart_t)GetProcAddress(m_hDLL, "Do_A2_Speedflash_Start");
    A2Command.SpeedflashSetLastBlock_Fn     = (A2SpeedflashSetLastBlock_t)GetProcAddress(m_hDLL, "Do_A2_Speedflash_SetLastBlock");
    A2Command.SpeedflashWriteBlock_Fn       = (A2SpeedflashWriteBlock_t)GetProcAddress(m_hDLL, "Do_A2_Speedflash_WriteBlock");

    if (
        Communication.Initialize_Fn          == 0 ||
        Communication.Shutdown_Fn            == 0 ||
        Communication.Poll_Fn                == 0 ||
        Communication.SetFamily_Fn           == 0 ||
        Communication.Send_Fn                == 0 ||
        Communication.SetProtocolTimeouts_Fn == 0 ||
        Communication.GetProtocolTimeouts_Fn == 0 ||
        Communication.GetVersion_Fn          == 0 ||
        R15Command.Send_Fn                   == 0 ||
        R15Command.ResetSessionCounters_Fn   == 0 ||
        R15Bulk.OpenSession_Fn               == 0 ||
        R15Bulk.CreateVector_Fn              == 0 ||
        R15Bulk.DestroyVector_Fn             == 0 ||
        R15Bulk.StartSession_Fn              == 0 ||
        R15Bulk.GetStatusSession_Fn          == 0 ||
        R15Bulk.CloseSession_Fn              == 0 ||
        R15Bulk.SetCallbacks_Fn              == 0 ||
        A2Command.Send_Fn                    == 0 ||
        A2Command.SpeedflashStart_Fn         == 0 ||
        A2Command.SpeedflashSetLastBlock_Fn  == 0 ||
        A2Command.SpeedflashWriteBlock_Fn    == 0
    ) {
        return LCM_DLL_LOAD_FUNCTION_NOT_FOUND;
    }

    return E_SUCCESS;
}


int LcmInterface::SetLCMLibPath(const char *lcmLibPath)
{
    int ReturnValue = E_SUCCESS;
    size_t pathLength;
    VERIFY(lcmLibPath != NULL, E_INVALID_INPUT_PARAMETERS);

	pathLength = strlen(lcmLibPath);
    if (0 != m_pchLCMLibPath) {
        delete[] m_pchLCMLibPath;
        m_pchLCMLibPath = NULL;
    }

    m_pchLCMLibPath = new char[pathLength + 1];
    strcpy_s(m_pchLCMLibPath, pathLength + 1, lcmLibPath);
    m_pchLCMLibPath[pathLength] = '\0';

ErrorExit:
    return ReturnValue;
}

void LcmInterface::CloseLCMLibrary()
{
    if (0 != m_pchLCMLibPath) {
        delete[] m_pchLCMLibPath;
        m_pchLCMLibPath = NULL;
    }

    if (NULL != m_hDLL) {
#ifdef _WIN32
        FreeLibrary(m_hDLL);
#else
        dlclose(m_hDLL);
#endif
    }
}

ErrorCode_e LcmInterface::CommunicationInitialize(void *Object_p, Family_t Family, HashDevice_t *HashDevice_p, CommunicationDevice_t *CommunicationDevice_p, Do_CEH_Call_t CommandCallback_p, BuffersInterface_t *Buffers_p, TimersInterface_t *Timers_p, QueueInterface_t *Queue_p)
{
    int ReturnValue = E_SUCCESS;
    char *LCMVersion_p = NULL;
    LCM_t LCMType = PC_LCM;

    if (m_hDLL == NULL) {
        CLockCS lock(m_CriticalSection);
        VERIFY_SUCCESS(LoadLCMLibrary());
        LCMVersion_p = Communication.GetVersion_Fn(m_pCommunication);
        VERIFY_SUCCESS(CommunicationCheckVersion(LCMVersion_p, LCMType));
    }

    ReturnValue = Communication.Initialize_Fn(Object_p, &m_pCommunication, Family, HashDevice_p, CommunicationDevice_p, CommandCallback_p, Buffers_p, Timers_p, Queue_p);

ErrorExit:

    if (ReturnValue != E_SUCCESS) {
        CloseLCMLibrary();
        m_hDLL = NULL;
    }

    return static_cast<ErrorCode_e>(ReturnValue);
}

ErrorCode_e LcmInterface::CommunicationSend(void *InputData_p)
{
    return Communication.Send_Fn(m_pCommunication, InputData_p);
}

ErrorCode_e LcmInterface::CommunicationSetFamily(Family_t family, Do_CEH_Call_t CEHCallback)
{
    return Communication.SetFamily_Fn(m_pCommunication, family, CEHCallback);
}

ErrorCode_e LcmInterface::CommunicationSetProtocolTimeouts(void *TimeoutData_p)
{
    return Communication.SetProtocolTimeouts_Fn(m_pCommunication, TimeoutData_p);
}

ErrorCode_e LcmInterface::CommunicationGetProtocolTimeouts(void *TimeoutData_p)
{
    return Communication.GetProtocolTimeouts_Fn(m_pCommunication, TimeoutData_p);
}

ErrorCode_e LcmInterface::CommunicationCheckVersion(char *LCMVersion_p, LCM_t LCMType)
{

    int ReturnValue = LCM_LOAD_INCOMPATIBLE_PC_VERSION;

    if (LCMType == LDR_LCM) {
        ReturnValue = LCM_LOAD_INCOMPATIBLE_LDR_VERSION;
    }

    if (strcmp(LCMVersion_p, LCD_LCM_Compatibility) == 0) {
        ReturnValue = E_SUCCESS;
    }

    return static_cast<ErrorCode_e>(ReturnValue);
}

ErrorCode_e LcmInterface::CommunicationShutdown()
{
    if (Communication.Shutdown_Fn != NULL) {
        return Communication.Shutdown_Fn(&m_pCommunication);
    }

    return static_cast<ErrorCode_e>(E_SUCCESS);
}

ErrorCode_e LcmInterface::CommunicationCancelReceiver(uint8 PacketsBeforeReceiverStop)
{
    return Communication.CancelReceiver_Fn(m_pCommunication, PacketsBeforeReceiverStop);
}
ErrorCode_e LcmInterface::CommandSend(CommandData_t *CmdData_p)
{
    return R15Command.Send_Fn(m_pCommunication, CmdData_p);
}

ErrorCode_e LcmInterface::CommunicationPoll()
{
    return Communication.Poll_Fn(m_pCommunication);
}

ErrorCode_e LcmInterface::CommandResetSessionCounters()
{
    return R15Command.ResetSessionCounters_Fn(m_pCommunication);
}

void LcmInterface::BulkSetCallbacks(void *BulkCommandCallback_p, void *BulkDataCallback_p, void *BulkDataEndOfDump_p)
{
    R15Bulk.SetCallbacks_Fn(m_pCommunication, BulkCommandCallback_p, BulkDataCallback_p, BulkDataEndOfDump_p);
}

void LcmInterface::BulkBuffersRelease(void *BulkBufferRelease_p)
{
    R15Bulk.SetBuffersRelease_Fn(m_pCommunication, BulkBufferRelease_p);
}

uint32 LcmInterface::BulkOpenSession(const uint16 SessionId, const TL_SessionMode_t Mode, uint32 Length)
{
    return R15Bulk.OpenSession_Fn(m_pCommunication, SessionId, Mode, Length);
}

TL_BulkVectorList_t *LcmInterface::BulkCreateVector(const uint32 BulkVector, uint32 Length, const uint32 BuffSize, TL_BulkVectorList_t *CreatedBulkVector_p)
{
    return R15Bulk.CreateVector_Fn(m_pCommunication, BulkVector, Length, BuffSize, CreatedBulkVector_p);
}

ErrorCode_e LcmInterface::BulkStartSession(TL_BulkVectorList_t *BulkVector_p, const uint64 Offset)
{
    return R15Bulk.StartSession_Fn(m_pCommunication, BulkVector_p, Offset);
}

uint32 LcmInterface::BulkDestroyVector(TL_BulkVectorList_t *BulkVector_p, boolean ReqReleaseBuffer)
{
    return R15Bulk.DestroyVector_Fn(m_pCommunication, BulkVector_p, ReqReleaseBuffer);
}

ErrorCode_e LcmInterface::BulkCloseSession(TL_BulkVectorList_t *BulkVector_p)
{
    return R15Bulk.CloseSession_Fn(m_pCommunication, BulkVector_p);
}

ErrorCode_e LcmInterface::A2CommandSend(A2_CommandData_t *CmdData_p)
{
    return A2Command.Send_Fn(m_pCommunication, CmdData_p);
}

void LcmInterface::A2SpeedflashStart()
{
    A2Command.SpeedflashStart_Fn(m_pCommunication);
}

void LcmInterface::A2SpeedflashSetLastBlock()
{
    A2Command.SpeedflashSetLastBlock_Fn(m_pCommunication);
}

ErrorCode_e LcmInterface::A2SpeedflashWriteBlock(const void *Buffer, const size_t BufferSize)
{
    return A2Command.SpeedflashWriteBlock_Fn(m_pCommunication, Buffer, BufferSize);
}
