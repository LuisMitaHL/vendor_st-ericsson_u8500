/******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
******************************************************************************/

#include "BulkHandler.h"
#include "LCDriverMethods.h"
#include "Buffers.h"
#include "LcmInterface.h"
#include "Logger.h"
#include "Error.h"
#include "lcdriver_error_codes.h"
#include <string>
#include <cstdio>
using namespace std;

/// <summary>
/// BulkHandler constructor
/// </summary>
BulkHandler::BulkHandler(CLCDriverMethods *methods, Buffers *buffers, LcmInterface *lcmInterface, Logger *logger):
    m_Methods(methods),
    m_ReceiveQueue(32),
    m_pBuffers(buffers),
    m_pLcmInterface(lcmInterface),
    m_pLogger(logger),
    m_State(BULK_INACTIVE),
    m_pFileWriteThread(0),
    m_pBulkVector(0)
{
}

/// <summary>
/// BulkHandler destructor
/// </summary>
BulkHandler::~BulkHandler()
{
    TL_BulkVectorList_t *bulkVector = 0;

    while (true) {
        RemoveResult result = m_ReceiveQueue.RemoveHead(reinterpret_cast<void **>(&bulkVector), 0);

        if (REMOVE_TIMEOUT == result) {
            break;
        } else if (REMOVE_CANCEL == result) {
            continue;
        }

        m_pLcmInterface->BulkDestroyVector(bulkVector, true);
#ifdef _BULKDEBUG
        m_pLogger->log("BULK: Vector destroyed BulkVector = 0x%x", m_pBulkVector);
#endif
    }
}

int BulkHandler::Send(const string &sourceFile)
{
    if (BULK_INACTIVE != m_State) {
        return BULK_ALREADY_IN_PROGRESS;
    }

    m_sFilePath = sourceFile;
    m_State = BULK_TX;
    return 0;
}

int BulkHandler::Receive(const string &destinationFile)
{
    if (BULK_INACTIVE != m_State) {
        return BULK_ALREADY_IN_PROGRESS;
    }

    m_sFilePath = destinationFile;

    // truncate the file for receiving
    FILE *file = fopen(m_sFilePath.c_str(), "wb");

    if (!file) {
        return FILE_OPENING_ERROR;
    }

    fclose(file);

    m_State = BULK_RX;

    // start bulk file write thread
    m_pFileWriteThread = new CThreadWrapper(FileWriteThreadFunction, this);
    m_pFileWriteThread->ResumeThread();

    return 0;
}

void BulkHandler::Finish(bool ForceFinish)
{
    if (BULK_RX == m_State) {
        m_State = BULK_INACTIVE;
        m_ReceiveQueue.SignalEvent();
        m_pFileWriteThread->WaitToDie(INFINITE);
    } else if (m_State != BULK_INACTIVE) {
        m_State = BULK_INACTIVE;

        if (ForceFinish) {
            m_pBulkVector->State = CANCEL_BULK;
        }
    }
}

void BulkHandler::HandleCommandRequest(uint16 session, uint32 chunkSize, uint64 offset, uint32 length, bool acknowledge)
{
    switch (SELECT_COMMAND(m_State, acknowledge)) {
    case BULK_WRITE_REQUEST:
        HandleWriteRequest(session, chunkSize, offset, length);
        break;

    case BULK_READ_REQUEST:
        HandleReadRequest(session, chunkSize, offset, length);
        break;

    case BULK_RX_SESSION_END:
        HandleRxSessionEnd(session, chunkSize, offset, length);
        break;

    case BULK_TX_SESSION_END:
        HandleTxSessionEnd(session, chunkSize, offset, length);
        break;

    default:
        break;
    }
}

void BulkHandler::HandleWriteRequest(uint16 session, uint32 chunkSize, uint64 offset, uint32 length)
{
    int ReturnValue = E_SUCCESS;

    uint32 VectorID = m_pLcmInterface->BulkOpenSession(session, BULK_RECEIVE, length);
    VERIFY(BULK_ERROR != VectorID, BULK_VECTOR_ID_ERROR);
#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Session opened with VectorID = %u", VectorID);
#endif
    m_pBulkVector = m_pLcmInterface->BulkCreateVector(VectorID, length, chunkSize, NULL);
    VERIFY_SUCCESS(m_pLcmInterface->BulkStartSession(m_pBulkVector, offset));
#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Session %u started with BulkVector = 0x%x", session, m_pBulkVector);
#endif

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        m_Methods->SignalError(ReturnValue);
    }
}

void BulkHandler::HandleReadRequest(uint16 session, uint32 chunkSize, uint64 offset, uint32 length)
{
    int ReturnValue = E_SUCCESS;

    uint32 VectorID = m_pLcmInterface->BulkOpenSession(session, BULK_SEND, length);
    VERIFY(BULK_ERROR != VectorID, BULK_VECTOR_ID_ERROR);
#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Session opened with VectorID = %u", VectorID);
#endif
    m_pBulkVector = m_pLcmInterface->BulkCreateVector(VectorID, length, chunkSize, NULL);

    VERIFY_SUCCESS(m_pBuffers->AllocateBulkVector(m_pBulkVector, chunkSize, offset, length));
    VERIFY_SUCCESS(m_pLcmInterface->BulkStartSession(m_pBulkVector, offset));
#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Session %u started with BulkVector = 0x%x", session, m_pBulkVector);
#endif

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        m_Methods->SignalError(ReturnValue);
    }
}

void BulkHandler::HandleRxSessionEnd(uint16 session, uint32 chunkSize, uint64 offset, uint32 length)
{
    int ReturnValue = E_SUCCESS;

    m_ReceiveQueue.AddTail(m_pBulkVector);

    VERIFY_SUCCESS(m_pLcmInterface->BulkCloseSession(m_pBulkVector));

#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Session closed Session = %u", session);
#endif

    m_Methods->UpdateBulkProgress();

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        m_Methods->SignalError(ReturnValue);
    }
}

void BulkHandler::HandleTxSessionEnd(uint16 session, uint32 chunkSize, uint64 offset, uint32 length)
{
    int ReturnValue = E_SUCCESS;

    m_pBuffers->ReleaseBulkVector(m_pBulkVector);

    VERIFY_SUCCESS(m_pLcmInterface->BulkDestroyVector(m_pBulkVector, false));
#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Vector destroyed BulkVector = 0x%x", m_pBulkVector);
#endif

    m_Methods->UpdateBulkProgress();

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        m_Methods->SignalError(ReturnValue);
    }
}

void BulkHandler::FlushReceiveQueue()
{
#ifdef _BULKDEBUG
    m_pLogger->log("BULK: Flush receive queue");
#endif
    TL_BulkVectorList_t *bulkVector = 0;

    while (true) {
        RemoveResult result = m_ReceiveQueue.RemoveHead(reinterpret_cast<void **>(&bulkVector), 0);

        if (REMOVE_TIMEOUT == result) {
            break;
        } else if (REMOVE_CANCEL == result) {
            continue;
        }

        WriteBulkVector(bulkVector);

        m_pLcmInterface->BulkDestroyVector(bulkVector, true);
#ifdef _BULKDEBUG
        m_pLogger->log("BULK: Vector destroyed BulkVector = 0x%x", bulkVector);
#endif
    }
}

void BulkHandler::WriteBulkVector(TL_BulkVectorList_t *bulkVector)
{
    FILE *file = fopen(m_sFilePath.c_str(), "ab");

    if (file) {
        for (size_t i = 0; i != bulkVector->Buffers; ++i) {
            uint32 currentChunkSize = bulkVector->ChunkSize;

            if (i + 1 == bulkVector->Buffers) { //lastchunk
                currentChunkSize = bulkVector->Length - (i * bulkVector->ChunkSize);
            }

            fwrite(bulkVector->Entries[i].Payload_p, currentChunkSize, 1, file);
        }

        fclose(file);
#ifdef _BULKDEBUG
        m_pLogger->log("BULK: Vector written BulkVector = 0x%x", m_pBulkVector);
#endif
    }
}

#ifdef _WIN32
unsigned int WINAPI BulkHandler::FileWriteThreadFunction(void *arg)
#else
void *BulkHandler::FileWriteThreadFunction(void *arg)
#endif
{
    BulkHandler *pThis = static_cast<BulkHandler *>(arg);

    TL_BulkVectorList_t *bulkVector = 0;

    while (true) {
        RemoveResult result = pThis->m_ReceiveQueue.RemoveHead(reinterpret_cast<void **>(&bulkVector), INFINITE);

        if (REMOVE_SUCCESS == result) {
            pThis->WriteBulkVector(bulkVector);

            pThis->m_pLcmInterface->BulkDestroyVector(bulkVector, true);
#ifdef _BULKDEBUG
            pThis->m_pLogger->log("BULK: Vector destroyed BulkVector = 0x%x", bulkVector);
#endif
        } else {
            break;
        }
    }

    pThis->FlushReceiveQueue();

    return 0;
}
