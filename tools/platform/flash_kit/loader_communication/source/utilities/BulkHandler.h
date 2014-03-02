/******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
******************************************************************************/

#ifndef _BULK_HANDLER_H_
#define _BULK_HANDLER_H_

#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#endif
#include "Buffers.h"
#include "LcmInterface.h"
#include "Logger.h"
#include <string>

class CLCDriverMethods;

/// <summary>
/// Bulk handler state.
/// </summary>
enum BulkState {
    BULK_INACTIVE,
    BULK_RX,
    BULK_TX
};

#define SESSION_END_MASK (1 << 4)
#define SELECT_COMMAND(state, acknowledge) (state | (((uint32)acknowledge) << 4))

enum BulkCommand {
    BULK_WRITE_REQUEST  = BULK_RX,
    BULK_READ_REQUEST   = BULK_TX,
    BULK_RX_SESSION_END = BULK_WRITE_REQUEST | SESSION_END_MASK,
    BULK_TX_SESSION_END = BULK_READ_REQUEST | SESSION_END_MASK
};

/// <summary>
/// Bulk Buffer class used to manipulate
/// the bulk buffers linear list filled
/// with bulk chunks data
/// </summary>
class BulkHandler
{
public:
    BulkHandler(CLCDriverMethods *methods, Buffers *buffers, LcmInterface *lcmInterface, Logger *logger);
    ~BulkHandler();

    int Receive(const std::string &sourceFile);
    int Send(const std::string &destinationFile);
    void Finish(bool ForceFinish);
    void HandleCommandRequest(uint16 session, uint32 chunkSize, uint64 offset, uint32 length, bool acknowledge);
private:
    CLCDriverMethods *m_Methods;
    CSemaphoreQueue m_ReceiveQueue;
    Buffers *m_pBuffers;
    LcmInterface *m_pLcmInterface;
    Logger *m_pLogger;
    BulkState m_State;
    CThreadWrapper *m_pFileWriteThread;
    TL_BulkVectorList_t *m_pBulkVector;
    std::string m_sFilePath;

private:
    void HandleReadRequest(uint16 session, uint32 chunkSize, uint64 offset, uint32 length);
    void HandleWriteRequest(uint16 session, uint32 chunkSize, uint64 offset, uint32 length);
    void HandleRxSessionEnd(uint16 session, uint32 chunkSize, uint64 offset, uint32 length);
    void HandleTxSessionEnd(uint16 session, uint32 chunkSize, uint64 offset, uint32 length);
#ifdef _WIN32
    static unsigned int WINAPI FileWriteThreadFunction(void *arg);
#else
    static void *FileWriteThreadFunction(void *arg);
#endif
    void FlushReceiveQueue();
    void WriteBulkVector(TL_BulkVectorList_t *bulkVector);
};

#endif
