/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _LCMINTERFACE_H_
#define _LCMINTERFACE_H_

#include "t_communication_service.h"
#include "t_bulk_protocol.h"
#include "t_a2_protocol.h"

#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
typedef void *HMODULE;
#endif

extern char LCD_LCM_Compatibility[];

typedef ErrorCode_e(*CommunicationInitialize_t)(void *Object_p, Communication_t **Communication_pp, Family_t Family, HashDevice_t *HashDevice_p, CommunicationDevice_t *CommunicationDevice_p, Do_CEH_Call_t CommandCallback_p, BuffersInterface_t *Buffers_p, TimersInterface_t *Timers_p, QueueInterface_t *Queue_p);
typedef ErrorCode_e(*CommunicationShutdown_t)(Communication_t **Communication_pp);
typedef ErrorCode_e(*CommunicationPoll_t)(Communication_t *Communication_p);
typedef ErrorCode_e(*CommunicationSetFamily_t)(Communication_t *Communication_p, Family_t Family, Do_CEH_Call_t CEHCallback);
typedef ErrorCode_e(*CommunicationSend_t)(Communication_t *Communication_p, void *InputData_p);
typedef ErrorCode_e(*CommunicationSetProtocolTimeouts_t)(Communication_t *Communication_p, void *TimeoutData_p);
typedef ErrorCode_e(*CommunicationGetProtocolTimeouts_t)(Communication_t *Communication_p, void *TimeoutData_p);
typedef char *(*CommunicationGetVersion_t)(Communication_t *Communication_p);
typedef ErrorCode_e(*CommunicationCancelReceiver_t)(Communication_t *Communication_p, uint8 PacketsBeforeTransferStop);

typedef ErrorCode_e(*R15CommandSend_t)(Communication_t *Communication_p, CommandData_t *CmdData_p);
typedef ErrorCode_e(*R15CommandResetSessionCounters_t)(const Communication_t *const Communication_p);

typedef ErrorCode_e(*R15BulkStartSession_t)(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p, const uint64 Offset);
typedef ErrorCode_e(*R15BulkCloseSession_t)(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p);
typedef TL_BulkVectorList_t *(*R15BulkCreateVector_t)(const Communication_t *const Communication_p, const uint32 BulkVector, uint32 Length, const uint32 BuffSize, TL_BulkVectorList_t *CreatedBulkVector_p);
typedef TL_BulkSessionState_t (*R15BulkGetStatusSession_t)(const Communication_t *const Communication_p, const TL_BulkVectorList_t *BulkVector_p);
typedef uint32(*R15BulkDestroyVector_t)(const Communication_t *const Communication_p, TL_BulkVectorList_t *BulkVector_p, boolean ReqReleaseBuffer);
typedef uint32(*R15BulkOpenSession_t)(const Communication_t *const Communication_p, const uint16 SessionId, const TL_SessionMode_t Mode, uint32 Length);
typedef void (*R15BulkSetCallbacks_t)(Communication_t *Communication_p, void *BulkCommandCallback_p, void *BulkDataCallback_p, void *BulkDataEndOfDump_p);
typedef void (*R15BulkBuffersRelease_t)(Communication_t *Communication_p, void *BulkBuffersRelease_p);

typedef ErrorCode_e(*A2CommandSend_t)(Communication_t *Communication_p, A2_CommandData_t *CmdData_p);
typedef void (*A2SpeedflashStart_t)(Communication_t *Communication_p);
typedef void (*A2SpeedflashSetLastBlock_t)(Communication_t *Communication_p);
typedef ErrorCode_e(*A2SpeedflashWriteBlock_t)(Communication_t *Communication_p, const void *Buffer, const size_t BufferSize);

typedef struct {
    CommunicationInitialize_t           Initialize_Fn;
    CommunicationShutdown_t             Shutdown_Fn;
    CommunicationPoll_t                 Poll_Fn;
    CommunicationSetFamily_t            SetFamily_Fn;
    CommunicationSend_t                 Send_Fn;
    CommunicationSetProtocolTimeouts_t  SetProtocolTimeouts_Fn;
    CommunicationGetProtocolTimeouts_t  GetProtocolTimeouts_Fn;
    CommunicationGetVersion_t           GetVersion_Fn;
    CommunicationCancelReceiver_t       CancelReceiver_Fn;
} CommunicationInterface_t;

typedef struct {
    R15CommandSend_t                    Send_Fn;
    R15CommandResetSessionCounters_t    ResetSessionCounters_Fn;
} R15CommandInterface_t;

typedef struct {
    R15BulkStartSession_t           StartSession_Fn;
    R15BulkCloseSession_t           CloseSession_Fn;
    R15BulkCreateVector_t           CreateVector_Fn;
    R15BulkGetStatusSession_t       GetStatusSession_Fn;
    R15BulkDestroyVector_t          DestroyVector_Fn;
    R15BulkOpenSession_t            OpenSession_Fn;
    R15BulkSetCallbacks_t           SetCallbacks_Fn;
    R15BulkBuffersRelease_t         SetBuffersRelease_Fn;
} R15BulkInterface_t;

typedef struct {
    A2CommandSend_t             Send_Fn;
    A2SpeedflashStart_t         SpeedflashStart_Fn;
    A2SpeedflashSetLastBlock_t      SpeedflashSetLastBlock_Fn;
    A2SpeedflashWriteBlock_t        SpeedflashWriteBlock_Fn;
} A2CommandInterface_t;


typedef enum {
    UNKNOWN_LCM = 0,
    LDR_LCM = 1,
    PC_LCM = 2
} LCM_t;

class LcmInterface
{
public:
    LcmInterface();
    virtual ~LcmInterface();

    ErrorCode_e CommunicationInitialize(void *Object_p, Family_t Family, HashDevice_t *HashDevice_p, CommunicationDevice_t *CommunicationDevice_p, Do_CEH_Call_t CommandCallback_p, BuffersInterface_t *Buffers_p, TimersInterface_t *Timers_p, QueueInterface_t *Queue_p);
    ErrorCode_e CommunicationPoll();
    ErrorCode_e CommunicationSend(void *InputData_p);
    ErrorCode_e CommunicationSetFamily(Family_t family, Do_CEH_Call_t CEHCallback);
    ErrorCode_e CommunicationSetProtocolTimeouts(void *TimeoutData_p);
    ErrorCode_e CommunicationGetProtocolTimeouts(void *TimeoutData_p);
    ErrorCode_e CommunicationCheckVersion(char *LCMVersion_p, LCM_t LCM_type);
    ErrorCode_e CommunicationShutdown();
    ErrorCode_e CommunicationCancelReceiver(uint8 PacketsBeforeReceiverStop);

    ErrorCode_e CommandSend(CommandData_t *CmdData_p);
    ErrorCode_e CommandResetSessionCounters();

    void BulkSetCallbacks(void *BulkCommandCallback_p, void *BulkDataCallback_p, void *BulkDataEndOfDump_p);
    void BulkBuffersRelease(void *BulkBufferRelease_p);
    uint32 BulkOpenSession(const uint16 SessionId, const TL_SessionMode_t Mode, uint32 Length);
    TL_BulkVectorList_t *BulkCreateVector(const uint32 BulkVector, uint32 Length, const uint32 BuffSize, TL_BulkVectorList_t *CreatedBulkVector_p);
    ErrorCode_e BulkStartSession(TL_BulkVectorList_t *BulkVector_p, const uint64 Offset);
    uint32 BulkDestroyVector(TL_BulkVectorList_t *BulkVector_p, boolean ReqReleaseBuffer);
    ErrorCode_e BulkCloseSession(TL_BulkVectorList_t *BulkVector_p);

    ErrorCode_e A2CommandSend(A2_CommandData_t *CmdData_p);
    void A2SpeedflashStart();
    void A2SpeedflashSetLastBlock();
    ErrorCode_e A2SpeedflashWriteBlock(const void *Buffer, const size_t BufferSize);

    void *getLCMContext() {
        return m_pCommunication;
    }
private:
    Communication_t *m_pCommunication;
    ErrorCode_e lcmError_;
public:
    static int SetLCMLibPath(const char *lcmLibPath);
    static int LoadLCMLibrary();
    static void CloseLCMLibrary();
private:
    static CommunicationInterface_t Communication;
    static R15CommandInterface_t    R15Command;
    static R15BulkInterface_t       R15Bulk;
    static A2CommandInterface_t     A2Command;

    static HMODULE m_hDLL;
    static char *m_pchLCMLibPath;
    static CCriticalSectionObject   m_CriticalSection;
};

#endif // _LCMINTERFACE_H_
